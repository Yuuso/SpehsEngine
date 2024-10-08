#pragma once


namespace se
{
	namespace graphics
	{
		class Batch;
		class IndexBuffer;
		class Primitive;
		class VertexBuffer;

		class PrimitiveInternal
		{
		public:

			PrimitiveInternal(Primitive& _primitive);
			~PrimitiveInternal();

			PrimitiveInternal(const PrimitiveInternal& _other) = delete;
			PrimitiveInternal& operator=(const PrimitiveInternal& _other) = delete;

			PrimitiveInternal(PrimitiveInternal&& _other) = delete;
			PrimitiveInternal& operator=(PrimitiveInternal&& _other) = delete;

			bool operator==(const Primitive& _other) const;


			void update();

			void render(RenderContext& _renderContext);
			void preRender(bool _forceAllUpdates);
			void postRender();

			void batch(Batch& _batch);
			void unbatch();

			bool isBatched() const;
			bool wasDestroyed() const;
			RenderInfo getRenderInfo() const;
			RenderInfo getCopyRenderInfo() const;
			bool getRenderState() const;
			RenderMode getRenderMode() const;
			const VertexBuffer* getVertices() const;
			const IndexBuffer* getIndices() const;

		private:

			void render(RenderContext& _renderContext, const RenderInfo& _renderInfo);

			const bool sizeInBatchChanged() const;
			void registerAsBufferObjectRenderer();
			void unregisterAsBufferObjectRenderer();
			void primitiveDestroyed();

			ScopedConnection primitiveDestroyedConnection;

			Primitive* primitive = nullptr;
			PrimitiveUpdateFlagsType cachedPrimitiveUpdateFlags = 0;
			bool verticesChanged = false;
			bool indicesChanged = false;

			Batch* primitiveBatch = nullptr;
			const BatchPosition* batchPosition = nullptr;
		};
	}
}
