#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInternal.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Graphics/Internal/LightBatch.h"
#include "SpehsEngine/Graphics/Types.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"


namespace se
{
	namespace graphics
	{
		PrimitiveInternal::PrimitiveInternal(Primitive& _primitive)
			: primitive(&_primitive)
		{
			primitiveDestroyedConnection = primitive->destroyedSignal.connect(boost::bind(&PrimitiveInternal::primitiveDestroyed, this));
		}
		PrimitiveInternal::~PrimitiveInternal()
		{
			// TODO: These asserts can trigger when deleting scene
			//se_assert(!primitiveBatch);
			//se_assert(!batchPosition);
			unregisterAsBufferObjectRenderer();
		}

		void PrimitiveInternal::primitiveDestroyed()
		{
			unregisterAsBufferObjectRenderer();
			primitive = nullptr;
		}

		bool PrimitiveInternal::operator==(const Primitive& _other) const
		{
			return &_other == primitive;
		}

		void PrimitiveInternal::registerAsBufferObjectRenderer()
		{
			if (primitive)
			{
				std::shared_ptr<VertexBuffer> vertices = primitive->getVertices();
				if (vertices)
					vertices->registerAsRenderer(reinterpret_cast<uintptr_t>(this));
				std::shared_ptr<IndexBuffer> indices = primitive->getIndices();
				if (indices)
					indices->registerAsRenderer(reinterpret_cast<uintptr_t>(this));
			}
		}
		void PrimitiveInternal::unregisterAsBufferObjectRenderer()
		{
			if (primitive)
			{
				std::shared_ptr<VertexBuffer> vertices = primitive->getVertices();
				if (vertices)
					vertices->unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
				std::shared_ptr<IndexBuffer> indices = primitive->getIndices();
				if (indices)
					indices->unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
			}
		}


		void PrimitiveInternal::update()
		{
			primitive->update();

			if (!getRenderState())
			{
				unregisterAsBufferObjectRenderer();
				if (isBatched())
					unbatch();
				return;
			}

			if (getRenderMode() == RenderMode::Static)
			{
				unregisterAsBufferObjectRenderer();
				if (isBatched())
				{
					if ((checkBit(cachedPrimitiveUpdateFlags, PrimitiveUpdateFlag::RenderInfoChanged) || sizeInBatchChanged()))
					{
						unbatch();
					}
					else
					{
						if (checkBit(cachedPrimitiveUpdateFlags, PrimitiveUpdateFlag::TransformChanged) || verticesChanged)
						{
							primitiveBatch->updateVertices(*batchPosition, *getVertices(), primitive->getTransformMatrices()[0], primitive->getNormalMatrices()[0]);
						}

						if (indicesChanged)
						{
							primitiveBatch->updateIndices(*batchPosition, *getIndices());
						}
					}
				}
			}
			else
			{
				se_assert(getRenderMode() == RenderMode::Dynamic);
				if (isBatched())
					unbatch();
				registerAsBufferObjectRenderer();
			}
		}

		void PrimitiveInternal::render(RenderContext& _renderContext)
		{
			render(_renderContext, getRenderInfo());
			if (primitive->getRenderCopy())
				render(_renderContext, getCopyRenderInfo());
		}
		void PrimitiveInternal::render(RenderContext& _renderContext, const RenderInfo& _renderInfo)
		{
			if (!_renderInfo.material)
			{
				log::warning("Material missing!");
				return;
			}
			if (!_renderInfo.material->getShader()->ready())
			{
				log::warning("Shader not ready for rendering!");
				return;
			}

			const UniformMatrices& transformMatrices = primitive->getTransformMatrices();
			bgfx::setTransform(reinterpret_cast<const void*>(transformMatrices.data()), static_cast<uint16_t>(transformMatrices.size()));
			_renderContext.defaultUniforms->setNormalMatrices(primitive->getNormalMatrices());

			_renderContext.defaultUniforms->setPrimitiveColor(_renderInfo.primitiveColor);
			if (_renderInfo.material->getLit())
				_renderContext.lightBatch->bind();
			_renderInfo.material->bind();

			bgfx::IndexBufferHandle ibh = { getIndices()->bufferObject };
			bgfx::VertexBufferHandle vbh = { getVertices()->bufferObject };
			bgfx::setIndexBuffer(ibh);
			bgfx::setVertexBuffer(0, vbh);

			const std::vector<PrimitiveInstance>& instances = primitive->getInstances();
			if (instances.size() > 0)
			{
				constexpr uint16_t dataStride = sizeof(glm::mat4);
				const uint32_t numInstances = static_cast<uint32_t>(instances.size());
				if (bgfx::getAvailInstanceDataBuffer(numInstances, dataStride) == numInstances)
				{
					bgfx::InstanceDataBuffer idb;
					bgfx::allocInstanceDataBuffer(&idb, numInstances, dataStride);

					uint8_t* data = idb.data;
					for (auto&& instance : instances)
					{
						glm::mat4* transform = reinterpret_cast<glm::mat4*>(data);
						*transform = glm::translate(instance.position) * glm::mat4_cast(instance.rotation) * glm::scale(glm::vec3(instance.scale));
						data += dataStride;
					}

					bgfx::setInstanceDataBuffer(&idb);
				}
				else
				{
					log::error("No instance buffer slots available!");
				}
			}

			applyRenderState(_renderInfo, _renderContext);

			bgfx::ProgramHandle programHandle = { _renderInfo.material->getShader()->getHandle() };
			bgfx::submit(_renderContext.currentViewId, programHandle);
		}

		void PrimitiveInternal::preRender(const bool _forceAllUpdates)
		{
			if (_forceAllUpdates)
			{
				enableBit(cachedPrimitiveUpdateFlags, PrimitiveUpdateFlag::EverythingChanged);
				verticesChanged = true;
				indicesChanged = true;
			}
			else
			{
				enableBit(cachedPrimitiveUpdateFlags, primitive->updateFlags);
				verticesChanged = verticesChanged || primitive->getVerticesChanged();
				indicesChanged = indicesChanged || primitive->getIndicesChanged();
			}
		}
		void PrimitiveInternal::postRender()
		{
			if (getRenderState())
			{
				cachedPrimitiveUpdateFlags = 0;
				se_assert(primitive->updateFlags == 0);
				verticesChanged = false;
				indicesChanged = false;
				se_assert(!primitive->getVerticesChanged());
				se_assert(!primitive->getIndicesChanged());
			}
		}

		void PrimitiveInternal::batch(Batch& _batch)
		{
			se_assert(!isBatched());
			batchPosition = &_batch.add(*getVertices(), *getIndices());
			primitiveBatch = &_batch;
			if (primitive->getTransformMatrices()[0] != glm::mat4(1.0f)) // No need to update with identity matrix
				primitiveBatch->updateVertices(*batchPosition, *getVertices(), primitive->getTransformMatrices()[0], primitive->getNormalMatrices()[0]);
		}
		void PrimitiveInternal::unbatch()
		{
			se_assert(isBatched());
			primitiveBatch->remove(*batchPosition);
			primitiveBatch = nullptr;
			batchPosition = nullptr;
		}

		const bool PrimitiveInternal::isBatched() const
		{
			return primitiveBatch != nullptr;
		}
		const bool PrimitiveInternal::wasDestroyed() const
		{
			return primitive == nullptr;
		}
		const RenderInfo PrimitiveInternal::getRenderInfo() const
		{
			RenderInfo result;
			result.renderFlags = primitive->getRenderFlags();
			result.primitiveType = primitive->getPrimitiveType();
			result.material = primitive->getMaterial();
			result.attributes = primitive->getVertices()->getAttributes();
			result.primitiveColor = primitive->getColor();
			return result;
		}
		const RenderInfo PrimitiveInternal::getCopyRenderInfo() const
		{
			const RenderCopy* renderCopy = primitive->getRenderCopy();
			if (!renderCopy)
			{
				log::error("Render copy not set!");
				return getRenderInfo();
			}
			RenderInfo result;
			result.renderFlags = renderCopy->renderFlags;
			result.primitiveType = renderCopy->primitiveType;
			result.material = primitive->getMaterial();
			result.attributes = primitive->getVertices()->getAttributes();
			result.primitiveColor = renderCopy->primitiveColor;
			return result;
		}
		const bool PrimitiveInternal::getRenderState() const
		{
			return primitive->getRenderState()
				&& getVertices()
				&& getVertices()->size() > 0
				&& getIndices()
				&& getIndices()->size() > 0
				&& primitive->getMaterial();
		}
		const RenderMode PrimitiveInternal::getRenderMode() const
		{
			return primitive->getRenderMode();
		}
		const VertexBuffer* PrimitiveInternal::getVertices() const
		{
			// const, to prevent bufferChanged modification internally
			return primitive->getVertices().get();
		}
		const IndexBuffer* PrimitiveInternal::getIndices() const
		{
			// const, to prevent bufferChanged modification internally
			return primitive->getIndices().get();
		}

		const bool PrimitiveInternal::sizeInBatchChanged() const
		{
			se_assert(isBatched());
			if (verticesChanged || indicesChanged)
			{
				return (primitive->getVertices()->size() != (batchPosition->verticesEnd - batchPosition->verticesStart)) ||
					   (primitive->getIndices()->size() != (batchPosition->indicesEnd - batchPosition->indicesStart));
			}
			return false;
		}
	}
}
