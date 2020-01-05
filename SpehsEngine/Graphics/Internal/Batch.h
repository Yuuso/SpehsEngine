#pragma once

#include "SpehsEngine/Graphics/Internal/BatchPosition.h"
#include "SpehsEngine/Graphics/Internal/RenderContext.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/Types.h"
#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!
#include <vector>


namespace se
{
	namespace graphics
	{
		class Batch
		{
		public:

			Batch(const RenderFlagsType _renderFlags, const Shader& _shader);
			~Batch();

			Batch(const Batch& _other) = delete;
			Batch& operator=(const Batch& _other) = delete;

			Batch(Batch&& _other) = delete;
			Batch& operator=(Batch&& _other) = delete;


			bool check(const RenderFlagsType _renderFlags) const;
			bool check(const size_t numVertices, const size_t numIndices) const;

			[[nodiscard]] BatchPosition add(const std::vector<Vertex>& _vertices, const std::vector<IndexType>& _indices);
			void remove(const BatchPosition& _positionInBatch);

			void render(RenderContext& _renderContext) const;

		private:

			void updateBuffers();

			const RenderFlagsType renderFlags;
			const Shader& shader;

			std::vector<Vertex> vertices;
			std::vector<IndexType> indices;

			bgfx::DynamicVertexBufferHandle vertexBufferHandle = BGFX_INVALID_HANDLE;
			bgfx::DynamicIndexBufferHandle indexBufferHandle = BGFX_INVALID_HANDLE;
		};
	}
}
