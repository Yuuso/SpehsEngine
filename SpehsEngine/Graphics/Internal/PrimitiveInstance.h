#pragma once

#include "bgfx/bgfx.h" // TODO: try to get rid of this
#include "glm/mat4x4.hpp"
#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/BatchPosition.h"
#include "SpehsEngine/Graphics/Internal/RenderContext.h"
#include "SpehsEngine/Graphics/Primitive.h"


namespace se
{
	namespace graphics
	{
		class PrimitiveInstance
		{
		public:

			PrimitiveInstance(const Primitive& _primitive);
			~PrimitiveInstance();

			PrimitiveInstance(const PrimitiveInstance& _other) = delete;
			PrimitiveInstance& operator=(const PrimitiveInstance& _other) = delete;

			PrimitiveInstance(PrimitiveInstance&& _other) = delete;
			PrimitiveInstance& operator=(PrimitiveInstance&& _other) = delete;

			bool operator==(const Primitive& _other);


			void destroyBuffers();
			void render(RenderContext& _renderContext);

			void updateTransformMatrix();


			const Primitive& primitive;

			glm::mat4 transformMatrix;

			Batch* batch = nullptr;
			BatchPosition batchPosition;

			bgfx::VertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;
			bgfx::IndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;
		};
	}
}
