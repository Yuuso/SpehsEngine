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
				getVertices().registerAsRenderer(reinterpret_cast<uintptr_t>(this));
				getIndices().registerAsRenderer(reinterpret_cast<uintptr_t>(this));
			}
		}
		void PrimitiveInstance::unregisterAsBufferObjectRenderer()
		{
			if (primitive)
			{
				getVertices().unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
				getIndices().unregisterAsRenderer(reinterpret_cast<uintptr_t>(this));
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
						if (checkBit(cachedPrimitiveUpdateFlags, PrimitiveUpdateFlag::TransformChanged))
						{
							primitiveBatch->updateVertices(*batchPosition, getVertices(), primitive->getTransformMatrix(), primitive->getNormalMatrix());
						}
						else if (checkBit(primitive->updateFlags, PrimitiveUpdateFlag::VerticesChanged))
						{
							primitiveBatch->updateVertices(*batchPosition, getVertices(), primitive->getTransformMatrix(), primitive->getNormalMatrix());
						}

						if (checkBit(primitive->updateFlags, PrimitiveUpdateFlag::IndicesChanged))
						{
							primitiveBatch->updateIndices(*batchPosition, getIndices());
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
			const RenderInfo renderInfo = getRenderInfo();

			if (!renderInfo.material)
			{
				log::warning("Material missing!");
				return;
			}
			if (!renderInfo.material->getShader()->ready())
			{
				log::warning("Shader not ready for rendering!");
				return;
			}

			bgfx::setTransform(reinterpret_cast<const void*>(&primitive->getTransformMatrix()));
			_renderContext.defaultUniforms->setNormalMatrix(primitive->getNormalMatrix());
			_renderContext.lightBatch->bind();
			renderInfo.material->bind();

			bgfx::IndexBufferHandle ibh = { getIndices().bufferObject };
			bgfx::VertexBufferHandle vbh = { getVertices().bufferObject };
			bgfx::setIndexBuffer(ibh);
			bgfx::setVertexBuffer(0, vbh);

			applyRenderState(renderInfo, _renderContext);

			bgfx::ProgramHandle programHandle = { renderInfo.material->getShader()->getHandle() };
			bgfx::submit(_renderContext.currentViewId, programHandle);
		}

		void PrimitiveInstance::preRender()
		{
			enableBit(cachedPrimitiveUpdateFlags, primitive->updateFlags);
		}
		void PrimitiveInstance::postRender()
		{
			if (getRenderState())
			{
				cachedPrimitiveUpdateFlags = 0;
				se_assert(primitive->updateFlags == 0);
			}
		}

		void PrimitiveInstance::batch(Batch& _batch)
		{
			se_assert(!isBatched());
			batchPosition = &_batch.add(getVertices(), getIndices());
			primitiveBatch = &_batch;
			primitiveBatch->updateVertices(*batchPosition, getVertices(), primitive->getTransformMatrix(), primitive->getNormalMatrix());
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
			result.attributes = primitive->getVertices().getAttributes();
			return result;
		}
		const bool PrimitiveInstance::getRenderState() const
		{
			return primitive->getRenderState() && getVertices().size() > 0 && getIndices().size() > 0 && primitive->material != nullptr;
		}
		const RenderMode PrimitiveInstance::getRenderMode() const
		{
			return primitive->getRenderMode();
		}
		const VertexBuffer& PrimitiveInstance::getVertices() const
		{
			return primitive->getVertices();
		}
		const IndexBuffer& PrimitiveInstance::getIndices() const
		{
			return primitive->getIndices();
		}

		const bool PrimitiveInstance::sizeInBatchChanged() const
		{
			se_assert(isBatched());
			if (checkBit(primitive->updateFlags, PrimitiveUpdateFlag::VerticesChanged) ||
				checkBit(primitive->updateFlags, PrimitiveUpdateFlag::IndicesChanged))
			{
				return (primitive->getVertices().size() == (batchPosition->verticesEnd - batchPosition->verticesStart)) &&
					   (primitive->getIndices().size() == (batchPosition->indicesEnd - batchPosition->indicesStart));
			}
			return false;
		}
	}
}
