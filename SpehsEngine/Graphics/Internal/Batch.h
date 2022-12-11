#pragma once

#include "SpehsEngine/Graphics/IndexBuffer.h"
#include "SpehsEngine/Graphics/Internal/RenderInfo.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"


namespace se
{
	namespace graphics
	{
		class Batch
		{
		public:

			Batch(const RenderInfo _renderInfo);
			~Batch();

			Batch(const Batch& _other) = delete;
			Batch& operator=(const Batch& _other) = delete;

			Batch(Batch&& _other) = delete;
			Batch& operator=(Batch&& _other) = delete;


			bool check(const RenderInfo _renderInfo) const;
			bool check(const size_t numVertices, const size_t numIndices) const;

			[[nodiscard]] const BatchPosition& add(const VertexBuffer& _vertices, const IndexBuffer& _indices);
			void remove(const BatchPosition& _positionInBatch);

			void updateVertices(const BatchPosition& _positionInBatch, const VertexBuffer& _vertices,
								const glm::mat4& _transformMatrix, const glm::mat4& _normalMatrix);
			void updateIndices(const BatchPosition& _positionInBatch, const IndexBuffer& _indices);

			bool render(RenderContext& _renderContext);

		private:

			void updateBuffers();

			const RenderInfo renderInfo;
			bool needsVertexBufferUpdate = false;
			bool needsIndexBufferUpdate = false;

			VertexBuffer vertices;
			IndexBuffer indices;
			std::vector<std::unique_ptr<BatchPosition>> batchPositions;
		};
	}
}
