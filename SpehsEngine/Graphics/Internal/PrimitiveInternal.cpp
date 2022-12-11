#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInternal.h"

#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/DefaultUniforms.h"
#include "SpehsEngine/Graphics/Internal/LightBatch.h"
#include "SpehsEngine/Graphics/IndexBuffer.h"
#include "SpehsEngine/Graphics/Material.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/RenderCopy.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"


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
			primitiveDestroyedConnection.disconnect();
		}

		bool PrimitiveInternal::operator==(const Primitive& _other) const
		{
			return &_other == primitive;
		}

		void PrimitiveInternal::registerAsBufferObjectRenderer()
		{
			if (primitive)
			{
				if (auto vertices = primitive->getVertices())
					vertices->registerAsRenderer(reinterpret_cast<uintptr_t>(this));
				if (auto indices = primitive->getIndices())
					indices->registerAsRenderer(reinterpret_cast<uintptr_t>(this));
				if (auto instances = primitive->getInstances())
					instances->registerAsRenderer(reinterpret_cast<uintptr_t>(this));
			}
		}
		void PrimitiveInternal::unregisterAsBufferObjectRenderer()
		{
			if (primitive)
			{
				if (auto vertices = primitive->getVertices())
					vertices->unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
				if (auto indices = primitive->getIndices())
					indices->unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
				if (auto instances = primitive->getInstances())
					instances->unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
			}
		}


		void PrimitiveInternal::update()
		{
			primitive->update();

			if (!getRenderState())
			{
				unregisterAsBufferObjectRenderer();
				if (isBatched())
				{
					unbatch();
				}
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
				{
					unbatch();
				}
				registerAsBufferObjectRenderer();
			}
		}

		void PrimitiveInternal::render(RenderContext& _renderContext)
		{
			render(_renderContext, getRenderInfo());
			if (primitive->getRenderCopy())
			{
				render(_renderContext, getCopyRenderInfo());
			}
		}
		void PrimitiveInternal::render(RenderContext& _renderContext, const RenderInfo& _renderInfo)
		{
			if (!_renderInfo.material)
			{
				log::warning("Material missing!");
				return;
			}

			std::shared_ptr<VertexBuffer> instances = primitive->getInstances();
			const bool isInstanced = instances != nullptr;
			if (isInstanced && instances->size() == 0)
			{
				// Instance buffer empty, no need to render
				return;
			}

			const UniformMatrices& transformMatrices = primitive->getTransformMatrices();
			const bool isSkinned = transformMatrices.size() > 1;
			const bool isBillboardSpherical = checkBit(_renderInfo.renderFlags, RenderFlag::BillboardSpherical);
			const bool isBillboardCylindrical = checkBit(_renderInfo.renderFlags, RenderFlag::BillboardCylindrical);

			const ShaderVariant shaderVariant = getShaderVariant(isInstanced, isSkinned, isBillboardSpherical || isBillboardCylindrical);
			std::shared_ptr<Shader> shader = _renderInfo.material->getShader(shaderVariant);

			if (!shader)
			{
				log::warning("Shader missing!");
				return;
			}
			if (!shader->ready())
			{
				log::warning("Shader not ready for rendering!");
				return;
			}

			bgfx::setTransform(reinterpret_cast<const void*>(transformMatrices.data()), static_cast<uint16_t>(transformMatrices.size()));
			_renderContext.defaultUniforms->setNormalMatrices(primitive->getNormalMatrices());

			_renderContext.defaultUniforms->setPrimitiveColor(_renderInfo.primitiveColor);
			if (_renderInfo.material->getLit())
			{
				_renderContext.lightBatch->bind();
			}
			_renderInfo.material->bind();

			if (isBillboardSpherical || isBillboardCylindrical)
			{
				_renderContext.defaultUniforms->setBillboardInfo(primitive->getPosition(), isBillboardCylindrical);
			}

			bgfx::IndexBufferHandle ibh = { getIndices()->bufferObject };
			bgfx::VertexBufferHandle vbh = { getVertices()->bufferObject };
			bgfx::setIndexBuffer(ibh);
			bgfx::setVertexBuffer(0, vbh);

			if (isInstanced)
			{
				bgfx::VertexBufferHandle dvbh = { instances->bufferObject };
				bgfx::setInstanceDataBuffer(dvbh, 0, static_cast<uint32_t>(instances->size()));
			}

			const Scissor& scissor = primitive->getScissor();
			if (scissor.enabled)
			{
				bgfx::setScissor(scissor.x, scissor.y, scissor.width, scissor.height);
			}

			applyRenderState(_renderInfo, _renderContext);

			bgfx::setStencil(getStencilState(primitive->getStencil()));

			bgfx::ProgramHandle programHandle = { shader->getHandle() };
			bgfx::submit(_renderContext.currentViewId, programHandle, _renderInfo.depth, BGFX_DISCARD_ALL);
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
			{
				primitiveBatch->updateVertices(*batchPosition, *getVertices(), primitive->getTransformMatrices()[0], primitive->getNormalMatrices()[0]);
			}
		}
		void PrimitiveInternal::unbatch()
		{
			se_assert(isBatched());
			primitiveBatch->remove(*batchPosition);
			primitiveBatch = nullptr;
			batchPosition = nullptr;
		}

		bool PrimitiveInternal::isBatched() const
		{
			return primitiveBatch != nullptr;
		}
		bool PrimitiveInternal::wasDestroyed() const
		{
			return primitive == nullptr;
		}
		RenderInfo PrimitiveInternal::getRenderInfo() const
		{
			RenderInfo result;
			result.renderFlags = primitive->getRenderFlags();
			result.primitiveType = primitive->getPrimitiveType();
			result.material = primitive->getMaterial();
			result.attributes = primitive->getVertices()->getAttributes();
			result.primitiveColor = primitive->getColor();
			result.depth = primitive->getRenderSortDepth();
			return result;
		}
		RenderInfo PrimitiveInternal::getCopyRenderInfo() const
		{
			const RenderCopy* renderCopy = primitive->getRenderCopy();
			if (!renderCopy)
			{
				log::error("Render copy not set!");
				return getRenderInfo();
			}
			RenderInfo result;
			result.renderFlags =
				renderCopy->renderFlags.has_value()
				? renderCopy->renderFlags.value()
				: primitive->getRenderFlags();
			result.primitiveType =
				renderCopy->primitiveType.has_value()
				? renderCopy->primitiveType.value()
				: primitive->primitiveType;
			result.material =
				renderCopy->material
				? renderCopy->material
				: primitive->getMaterial();
			result.attributes = primitive->getVertices()->getAttributes();
			result.primitiveColor =
				renderCopy->primitiveColor.has_value()
				? renderCopy->primitiveColor.value()
				: primitive->getColor();
			result.depth =
				renderCopy->renderDepth.has_value()
				? renderCopy->renderDepth.value()
				: primitive->getRenderSortDepth();
			return result;
		}
		bool PrimitiveInternal::getRenderState() const
		{
			return primitive->getRenderState()
				&& getVertices()
				&& getVertices()->size() > 0
				&& getIndices()
				&& getIndices()->size() > 0
				&& primitive->getMaterial();
		}
		RenderMode PrimitiveInternal::getRenderMode() const
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
