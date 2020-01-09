#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/PrimitiveInstance.h"

#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Core/SE_Assert.h"

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

		void PrimitiveInstance::destroyBuffers()
		{
			if (bgfx::isValid(vertexBufferHandle))
				bgfx::destroy(vertexBufferHandle);
			if (bgfx::isValid(indexBufferHandle))
				bgfx::destroy(indexBufferHandle);
		}
		void PrimitiveInstance::render(RenderContext& _renderContext)
		{
			se_assert(primitive);
			se_assert(primitive->getIndices().size() > 0 && primitive->getVertices().size() > 0);
			if (!bgfx::isValid(vertexBufferHandle))
			{
				const std::vector<Vertex>& vertices = primitive->getVertices();
				const bgfx::Memory* bufferMemory = bgfx::copy(&vertices[0], vertices.size() * sizeof(vertices[0]));
				vertexBufferHandle = bgfx::createVertexBuffer(bufferMemory, Vertex::getVertexLayout());
			}
			if (!bgfx::isValid(indexBufferHandle))
			{
				const std::vector<IndexType>& indices = primitive->getIndices();
				const bgfx::Memory* bufferMemory = bgfx::copy(&indices[0], indices.size() * sizeof(indices[0]));
				indexBufferHandle = bgfx::createIndexBuffer(bufferMemory);
				static_assert(sizeof(IndexType) == 2);
			}

			bgfx::setTransform(&transformMatrix[0][0]);

			bgfx::setIndexBuffer(indexBufferHandle);
			bgfx::setVertexBuffer(0, vertexBufferHandle);

			bgfx::setState(0
						   | BGFX_STATE_BLEND_ALPHA
						   | BGFX_STATE_WRITE_RGB
						   | BGFX_STATE_WRITE_A
						   | BGFX_STATE_WRITE_Z
						   | BGFX_STATE_DEPTH_TEST_LESS
//						   | BGFX_STATE_CULL_CCW
						   | BGFX_STATE_MSAA);

			bgfx::submit(_renderContext.currentViewId, primitive->shader->programHandle);
		}

		void PrimitiveInstance::updateTransformMatrix()
		{
			se_assert(primitive);
			transformMatrix = glm::translate(primitive->getLocalPosition() + primitive->initialLocalPosition) * glm::mat4_cast(primitive->initialLocalRotation) * glm::scale(primitive->initialLocalScale);
			transformMatrix = glm::translate(primitive->getPosition()) * glm::mat4_cast(primitive->getRotation()) * glm::scale(primitive->getScale()) * transformMatrix;
		}
	}
}
