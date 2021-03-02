#pragma once

#include "bgfx/bgfx.h" // ?!
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Graphics/ResourceHandle.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "glm/mat4x4.hpp"
#include <vector>
#include <memory>


namespace se
{
	namespace graphics
	{
		struct ResourceData
		{
			virtual ~ResourceData() = default;
			ResourceHandle handle = INVALID_RESOURCE_HANDLE;
		};


		struct ShaderData : ResourceData
		{
			ResourceHandle vertexShaderHandle = INVALID_RESOURCE_HANDLE;
			ResourceHandle fragmentShaderHandle = INVALID_RESOURCE_HANDLE;
		};

		struct TextureData : ResourceData
		{
			bgfx::TextureInfo info;
		};

		struct FontData : ResourceData
		{
			GlyphMap glyphMap;
			Rectangle fillerGlyph;
			FontMetrics fontMetrics;
		};

		struct MeshData : ResourceData
		{
			struct ModelNode
			{
				std::string name;
				glm::mat4 transform;
				std::vector<size_t> meshIndices;
				std::vector<std::unique_ptr<ModelNode>> children;
			};
			ModelNode rootNode;

			struct MeshInfo
			{
				PrimitiveType type;
				size_t materialIndex;
				std::shared_ptr<VertexBuffer> vertexBuffer;
				std::shared_ptr<IndexBuffer> indexBuffer;
			};
			std::vector<std::unique_ptr<MeshInfo>> meshes;
		};
	}
}
