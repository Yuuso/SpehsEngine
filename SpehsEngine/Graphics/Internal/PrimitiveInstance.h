#pragma once

#include "bgfx/bgfx.h" // TODO: try to get rid of this
#include "boost/signals2/connection.hpp"
#include "glm/mat4x4.hpp"
#include "SpehsEngine/Graphics/Internal/Batch.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Primitive.h"
#include "SpehsEngine/Graphics/IndexBuffer.h"
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

			void render(RenderContext& _renderContext);
			void preRender();
			void postRender();

			void batch(Batch& _batch);
			void unbatch();

			const bool isBatched() const;
			const bool wasDestroyed() const;
			const RenderInfo getRenderInfo() const;
			const bool getRenderState() const;
			const RenderMode getRenderMode() const;
			const VertexBuffer& getVertices() const;
			const IndexBuffer& getIndices() const;

		private:

			const bool sizeInBatchChanged() const;
			void registerAsBufferObjectRenderer();
			void unregisterAsBufferObjectRenderer();
			void primitiveDestroyed();

			boost::signals2::scoped_connection primitiveDestroyedConnection;

			Primitive* primitive = nullptr;
			PrimitiveUpdateFlagsType cachedPrimitiveUpdateFlags = 0;
			// TODO: weak_ptr?
			Batch* primitiveBatch = nullptr;
			const BatchPosition* batchPosition = nullptr;
		};
	}
}
