#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInstance.h"

#include "SpehsEngine/Core/BitwiseOperations.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"


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
			se_assert(!batch_);
			se_assert(!batchPosition);
			destroyBuffers();
		}

		void PrimitiveInstance::primitiveDestroyed()
		{
			primitive = nullptr;
		}

		bool PrimitiveInstance::operator==(const Primitive& _other) const
		{
			return &_other == primitive;
		}


		void PrimitiveInstance::update()
		{
			if (!getRenderState())
			{
				destroyBuffers();
				if (isBatched())
					unbatch();
				return;
			}

			if (getRenderMode() == RenderMode::Static)
			{
				destroyBuffers();
				if (isBatched())
				{
					const std::vector<Vertex>& vertices = getVertices();
					const std::vector<IndexType>& indices = getIndices();
					se_assert(vertices.size() != 0 && indices.size() != 0);
					if ((checkBit(primitive->updateFlags, UpdateFlag::RenderInfoChanged) || sizeInBatchChanged()))
					{
						unbatch();
					}
					else
					{
						if (checkBit(primitive->updateFlags, UpdateFlag::TransformChanged))
						{
							updateTransformMatrix();
							batch_->updateVertices(*batchPosition, vertices, transformMatrix);
						}
						else if (checkBit(primitive->updateFlags, UpdateFlag::VerticesChanged))
						{
							batch_->updateVertices(*batchPosition, vertices, transformMatrix);
						}

						if (checkBit(primitive->updateFlags, UpdateFlag::IndicesChanged))
						{
							batch_->updateIndices(*batchPosition, indices);
						}
					}
				}
			}
			else
			{
				if (isBatched())
					unbatch();
				if (checkBit(primitive->updateFlags, UpdateFlag::VerticesChanged) ||
					checkBit(primitive->updateFlags, UpdateFlag::IndicesChanged))
				{
					destroyBuffers();
				}
				if (checkBit(primitive->updateFlags, UpdateFlag::TransformChanged))
				{
					updateTransformMatrix();
				}
				// UpdateFlag::RenderInfoChanged doesn't matter
			}
			clearUpdateFlags();
		}

		void PrimitiveInstance::destroyBuffers()
		{
			// TODO: No need to destroy both buffers every time
			if (bgfx::isValid(vertexBufferHandle))
			{
				bgfx::destroy(vertexBufferHandle);
				vertexBufferHandle = BGFX_INVALID_HANDLE;
			}
			if (bgfx::isValid(indexBufferHandle))
			{
				bgfx::destroy(indexBufferHandle);
				indexBufferHandle = BGFX_INVALID_HANDLE;
			}
		}
		void PrimitiveInstance::render(RenderContext& _renderContext)
		{
			const std::vector<Vertex>& vertices = getVertices();
			const std::vector<IndexType>& indices = getIndices();

			se_assert(vertices.size() != 0 && indices.size() != 0);

			const RenderInfo renderInfo = getRenderInfo();

			bgfx::setTransform(&transformMatrix[0][0]);

			if (getRenderMode() == RenderMode::Transient)
			{
				// TODO: getAvailTransientVertexBuffer
				bgfx::TransientIndexBuffer transientIndexBuffer;
				bgfx::TransientVertexBuffer transientVertexBuffer;
				bgfx::allocTransientIndexBuffer(&transientIndexBuffer, indices.size());
				bgfx::allocTransientVertexBuffer(&transientVertexBuffer, vertices.size(), Vertex::getVertexLayout());
				memcpy(transientIndexBuffer.data, &indices[0], indices.size() * sizeof(indices[0]));
				memcpy(transientVertexBuffer.data, &vertices[0], vertices.size() * sizeof(vertices[0]));

				bgfx::setIndexBuffer(&transientIndexBuffer, 0, indices.size());
				bgfx::setVertexBuffer(0, &transientVertexBuffer, 0, vertices.size());
			}
			else
			{
				se_assert(primitive->getRenderMode() == RenderMode::Dynamic);

				if (!bgfx::isValid(vertexBufferHandle))
				{
					const bgfx::Memory* bufferMemory = bgfx::copy(&vertices[0], vertices.size() * sizeof(vertices[0]));
					vertexBufferHandle = bgfx::createVertexBuffer(bufferMemory, Vertex::getVertexLayout());
				}
				if (!bgfx::isValid(indexBufferHandle))
				{
					const bgfx::Memory* bufferMemory = bgfx::copy(&indices[0], indices.size() * sizeof(indices[0]));
					indexBufferHandle = bgfx::createIndexBuffer(bufferMemory);
					static_assert(sizeof(IndexType) == 2);
				}

				bgfx::setIndexBuffer(indexBufferHandle);
				bgfx::setVertexBuffer(0, vertexBufferHandle);
			}

			applyRenderState(getRenderInfo(), _renderContext);
			bgfx::submit(_renderContext.currentViewId, renderInfo.shader->programHandle);
		}

		void PrimitiveInstance::batch(Batch& _batch)
		{
			const std::vector<Vertex>& vertices = getVertices();
			const std::vector<IndexType>& indices = getIndices();
			se_assert(vertices.size() != 0 && indices.size() != 0);
			se_assert(!isBatched());
			batchPosition = &_batch.add(vertices, indices);
			batch_ = &_batch;
			updateTransformMatrix();
			batch_->updateVertices(*batchPosition, vertices, transformMatrix);
		}
		void PrimitiveInstance::unbatch()
		{
			se_assert(isBatched());
			batch_->remove(*batchPosition);
			batch_ = nullptr;
			batchPosition = nullptr;
		}

		void PrimitiveInstance::clearUpdateFlags()
		{
			primitive->updateFlags = UpdateFlag::NothingChanged;
		}

		const bool PrimitiveInstance::isBatched() const
		{
			return batch_ != nullptr;
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
			result.shader = primitive->getShader();
			return result;
		}
		const bool PrimitiveInstance::getRenderState() const
		{
			return primitive->getRenderState() && getVertices().size() > 0 && getIndices().size() > 0;
		}
		const RenderMode PrimitiveInstance::getRenderMode() const
		{
			return primitive->getRenderMode();
		}
		const std::vector<Vertex>& PrimitiveInstance::getVertices() const
		{
			return primitive->getVertices();
		}
		const std::vector<IndexType>& PrimitiveInstance::getIndices() const
		{
			return primitive->getIndices();
		}

		const bool PrimitiveInstance::sizeInBatchChanged() const
		{
			se_assert(isBatched());
			if (checkBit(primitive->updateFlags, UpdateFlag::VerticesChanged) ||
				checkBit(primitive->updateFlags, UpdateFlag::IndicesChanged))
			{
				return (primitive->getVertices().size() == (batchPosition->verticesEnd - batchPosition->verticesStart)) &&
					   (primitive->getIndices().size() == (batchPosition->indicesEnd - batchPosition->indicesStart));
			}
			return false;
		}
		void PrimitiveInstance::updateTransformMatrix()
		{
			transformMatrix = glm::translate(primitive->getLocalPosition()) *
							  glm::mat4_cast(primitive->getLocalRotation()) *
							  glm::scale(primitive->getLocalScale());
			transformMatrix = glm::translate(primitive->getPosition()) *
							  glm::mat4_cast(primitive->getRotation()) *
							  glm::scale(primitive->getScale()) *
							  transformMatrix;
		}
	}
}
