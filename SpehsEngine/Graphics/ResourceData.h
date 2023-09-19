#pragma once

#include "SpehsEngine/Graphics/FontMetrics.h"
#include "SpehsEngine/Graphics/ResourceHandle.h"


namespace se::gfx::impl
{
	struct Animation;
}

namespace se::gfx
{
	class IndexBuffer;
	class VertexBuffer;

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
		struct TextureInfo
		{
			uint16_t width;
			uint16_t height;
		};

		TextureInfo info;
	};

	struct FontData : ResourceData
	{
		GlyphMap glyphMap;
		Rectangle fillerGlyph;
		FontMetrics fontMetrics;
	};

	struct MeshData : ResourceData
	{
		struct MeshDataNode
		{
			std::string name;
			glm::mat4 transform;
			std::vector<size_t> meshIndices;
			std::vector<std::unique_ptr<MeshDataNode>> children;
		};
		MeshDataNode rootNode;

		struct BoneData
		{
			std::string boneNodeName;
			glm::mat4 offsetMatrix;
		};
		struct MeshInfo
		{
			std::string name;
			PrimitiveType type;
			size_t materialIndex;
			std::shared_ptr<VertexBuffer> vertexBuffer;
			std::shared_ptr<IndexBuffer> indexBuffer;
			std::vector<BoneData> bones;
		};
		std::vector<MeshInfo> meshes;

		glm::mat4 globalInverseMatrix;

		std::shared_ptr<std::vector<std::unique_ptr<impl::Animation>>> animations;
	};
}
