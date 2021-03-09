#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInstance.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Graphics/Internal/LightBatch.h"
#include "SpehsEngine/Graphics/Types.h"


namespace se
{
	namespace graphics
	{
		PrimitiveInstance::PrimitiveInstance(Primitive& _primitive)
			: primitive(&_primitive)
		{
			primitiveDestroyedConnection = primitive->destroyedSignal.connect(boost::bind(&PrimitiveInstance::primitiveDestroyed, this));
		}
		PrimitiveInstance::~PrimitiveInstance()
		{
			// TODO: These asserts can trigger when deleting scene
			//se_assert(!primitiveBatch);
			//se_assert(!batchPosition);
			unregisterAsBufferObjectRenderer();
		}

		void PrimitiveInstance::primitiveDestroyed()
		{
			unregisterAsBufferObjectRenderer();
			primitive = nullptr;
		}

		bool PrimitiveInstance::operator==(const Primitive& _other) const
		{
			return &_other == primitive;
		}

		void PrimitiveInstance::registerAsBufferObjectRenderer()
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
		void PrimitiveInstance::unregisterAsBufferObjectRenderer()
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


		void PrimitiveInstance::update()
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
							primitiveBatch->updateVertices(*batchPosition, *getVertices(), primitive->getTransformMatrix(), primitive->getNormalMatrix());
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

		void PrimitiveInstance::render(RenderContext& _renderContext)
		{
			render(_renderContext, getRenderInfo());
			if (primitive->getRenderCopy())
				render(_renderContext, getCopyRenderInfo());
		}
		void PrimitiveInstance::render(RenderContext& _renderContext, const RenderInfo& _renderInfo)
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

			bgfx::setTransform(reinterpret_cast<const void*>(&primitive->getTransformMatrix()));
			_renderContext.defaultUniforms->setNormalMatrix(primitive->getNormalMatrix());
			_renderContext.defaultUniforms->setPrimitiveColor(_renderInfo.primitiveColor);
			if (_renderInfo.material->getLit())
				_renderContext.lightBatch->bind();
			_renderInfo.material->bind();

			bgfx::IndexBufferHandle ibh = { getIndices()->bufferObject };
			bgfx::VertexBufferHandle vbh = { getVertices()->bufferObject };
			bgfx::setIndexBuffer(ibh);
			bgfx::setVertexBuffer(0, vbh);

			applyRenderState(_renderInfo, _renderContext);

			bgfx::ProgramHandle programHandle = { _renderInfo.material->getShader()->getHandle() };
			bgfx::submit(_renderContext.currentViewId, programHandle);
		}

		void PrimitiveInstance::preRender(const bool _forceAllUpdates)
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
		void PrimitiveInstance::postRender()
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

		void PrimitiveInstance::batch(Batch& _batch)
		{
			se_assert(!isBatched());
			batchPosition = &_batch.add(*getVertices(), *getIndices());
			primitiveBatch = &_batch;
			if (primitive->getTransformMatrix() != glm::mat4(1.0f)) // No need to update with identity matrix
				primitiveBatch->updateVertices(*batchPosition, *getVertices(), primitive->getTransformMatrix(), primitive->getNormalMatrix());
		}
		void PrimitiveInstance::unbatch()
		{
			se_assert(isBatched());
			primitiveBatch->remove(*batchPosition);
			primitiveBatch = nullptr;
			batchPosition = nullptr;
		}

		const bool PrimitiveInstance::isBatched() const
		{
			return primitiveBatch != nullptr;
		}
		const bool PrimitiveInstance::wasDestroyed() const
		{
			return primitive == nullptr;
		}
		const RenderInfo PrimitiveInstance::getRenderInfo() const
		{
			RenderInfo result;
			result.renderFlags = primitive->getRenderFlags();
			result.primitiveType = primitive->getPrimitiveType();
			result.material = primitive->getMaterial();
			result.attributes = primitive->getVertices()->getAttributes();
			result.primitiveColor = primitive->getColor();
			return result;
		}
		const RenderInfo PrimitiveInstance::getCopyRenderInfo() const
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
		const bool PrimitiveInstance::getRenderState() const
		{
			return primitive->getRenderState()
				&& getVertices()
				&& getVertices()->size() > 0
				&& getIndices()
				&& getIndices()->size() > 0
				&& primitive->getMaterial();
		}
		const RenderMode PrimitiveInstance::getRenderMode() const
		{
			return primitive->getRenderMode();
		}
		const VertexBuffer* PrimitiveInstance::getVertices() const
		{
			// const, to prevent bufferChanged modification internally
			return primitive->getVertices().get();
		}
		const IndexBuffer* PrimitiveInstance::getIndices() const
		{
			// const, to prevent bufferChanged modification internally
			return primitive->getIndices().get();
		}

		const bool PrimitiveInstance::sizeInBatchChanged() const
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
