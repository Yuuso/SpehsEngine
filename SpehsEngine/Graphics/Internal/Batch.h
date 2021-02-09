#pragma once

#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/RenderInfo.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!
#include "glm/mat4x4.hpp"
#include <vector>
#include <memory>


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

			[[nodiscard]] const BatchPosition& add(const VertexBuffer& _vertices, const std::vector<IndexType>& _indices);
			void remove(const BatchPosition& _positionInBatch);

			void updateVertices(const BatchPosition& _positionInBatch, const VertexBuffer& _vertices,
								const glm::mat4& _transformMatrix, const glm::mat4& _normalMatrix);
			void updateIndices(const BatchPosition& _positionInBatch, const std::vector<IndexType>& _indices);

			bool render(RenderContext& _renderContext);

		private:

			void updateBuffers();

			const RenderInfo renderInfo;
			bool needsVertexBufferUpdate = false;
			bool needsIndexBufferUpdate = false;

			VertexBuffer vertices;
			std::vector<IndexType> indices;
			std::vector<std::unique_ptr<BatchPosition>> batchPositions;

			bgfx::DynamicVertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;
			bgfx::DynamicIndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;
		};
	}
}
