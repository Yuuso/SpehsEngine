#pragma once

#include "bgfx/bgfx.h" // TODO: try to get rid of this
#include "glm/mat4x4.hpp"
#include "boost/signals2/connection.hpp"
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

			PrimitiveInstance(Primitive& _primitive);
			~PrimitiveInstance();

			PrimitiveInstance(const PrimitiveInstance& _other) = delete;
			PrimitiveInstance& operator=(const PrimitiveInstance& _other) = delete;

			PrimitiveInstance(PrimitiveInstance&& _other) = delete;
			PrimitiveInstance& operator=(PrimitiveInstance&& _other) = delete;

			bool operator==(const Primitive& _other) const;


			void render(RenderContext& _renderContext);

			void destroyBuffers();
			void updateTransformMatrix();


			// TODO: Privatize!
			Primitive* primitive;

			Batch* batch = nullptr;
			BatchPosition batchPosition;

		private:

			void primitiveDestroyed();

			boost::signals2::scoped_connection primitiveDestroyedConnection;

			glm::mat4 transformMatrix;

			bgfx::VertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;
			bgfx::IndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;
		};
	}
}
