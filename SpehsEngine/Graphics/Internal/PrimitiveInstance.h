#pragma once

#include "bgfx/bgfx.h" // TODO: try to get rid of this
#include "boost/signals2/connection.hpp"
#include "glm/mat4x4.hpp"
#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include <vector>

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


			void update();

			void destroyBuffers();
			void render(RenderContext& _renderContext);

			void batch(Batch& _batch);
			void unbatch();

			const bool isBatched() const;
			const bool wasDestroyed() const;
			const RenderInfo getRenderInfo() const;
			const bool getRenderState() const;
			const RenderMode getRenderMode() const;
			const VertexBuffer& getVertices() const;
			const std::vector<IndexType>& getIndices() const;

		private:

			const bool sizeInBatchChanged() const;
			void updateTransformMatrix();

			void primitiveDestroyed();

			boost::signals2::scoped_connection primitiveDestroyedConnection;

			Primitive* primitive;
			// TODO: weak_ptr?
			Batch* batch_ = nullptr;
			const BatchPosition* batchPosition = nullptr;

			glm::mat4 transformMatrix;

			// TODO: Test dynamic buffers?
			bgfx::VertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;
			bgfx::IndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;
		};
	}
}
