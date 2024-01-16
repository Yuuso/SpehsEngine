#pragma once

#include "SpehsEngine/Core/Asset.h"
#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/FontMetrics.h"
#include "SpehsEngine/Graphics/Impl/InternalUtilities.h"
#include "SpehsEngine/Math/RectanglePacker.h"
#include "bgfx/bgfx.h"


namespace se::gfx
{
	class IndexBuffer;
	class VertexBuffer;

	namespace impl
	{
		struct Animation;
	}
}

namespace se::gfx::impl
{
	class TextureData : public AssetData
	{
	public:

		uint16_t width = 0;
		uint16_t height = 0;

		TextureData()
		{
			handle.u16handle = bgfx::kInvalidHandle;
		}
		~TextureData()
		{
			safeDestroy<bgfx::TextureHandle>(handle);
		}
		void init(bgfx::TextureHandle _handle)
		{
			se_assert(bgfx::isValid(_handle));
			safeDestroy<bgfx::TextureHandle>(handle);
			handle.u16handle = _handle.idx;
		}
	};

	class FontData : public TextureData
	{
	public:

		GlyphMap glyphMap;
		Rectangle fillerGlyph;
		FontMetrics fontMetrics;
	};

	class ShaderData : public AssetData
	{
	public:

		AssetHandle vertexShaderHandle;
		AssetHandle fragmentShaderHandle;

		ShaderData()
		{
			handle.u16handle = bgfx::kInvalidHandle;
			vertexShaderHandle.u16handle = bgfx::kInvalidHandle;
			fragmentShaderHandle.u16handle = bgfx::kInvalidHandle;
		}
		~ShaderData()
		{
			safeDestroy<bgfx::ProgramHandle>(handle);
			safeDestroy<bgfx::ShaderHandle>(vertexShaderHandle);
			safeDestroy<bgfx::ShaderHandle>(fragmentShaderHandle);
		}
		void init(bgfx::ProgramHandle _programHandle,
			bgfx::ShaderHandle _vertexShader, bgfx::ShaderHandle _fragmentShader)
		{
			se_assert(bgfx::isValid(_programHandle));
			se_assert(bgfx::isValid(_vertexShader));
			se_assert(bgfx::isValid(_fragmentShader));
			safeDestroy<bgfx::TextureHandle>(handle);
			safeDestroy<bgfx::TextureHandle>(vertexShaderHandle);
			safeDestroy<bgfx::TextureHandle>(fragmentShaderHandle);
			handle.u16handle = _programHandle.idx;
			vertexShaderHandle.u16handle = _vertexShader.idx;
			fragmentShaderHandle.u16handle = _fragmentShader.idx;
		}
	};

	class MeshData : public AssetData
	{
	public:

		struct MeshDataNode
		{
			std::string name;
			glm::mat4 transform;
			std::vector<size_t> meshIndices;
			std::vector<std::unique_ptr<MeshDataNode>> children;
		};
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

		MeshDataNode rootNode;
		std::vector<MeshInfo> meshes;
		glm::mat4 globalInverseMatrix;
		std::shared_ptr<std::vector<std::unique_ptr<impl::Animation>>> animations;
	};
}
