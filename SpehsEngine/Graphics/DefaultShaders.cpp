#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultShaders.h"

#include "bgfx/embedded_shader.h"
#include "SpehsEngine/Core/EnumUtilityFunctions.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex_billboard.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex_billboard_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_skinned.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_skinned_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_text.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_skybox.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong_cubemap.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_text.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_cubemap.h"


static const bgfx::EmbeddedShader embeddedShaders[] =
{
	BGFX_EMBEDDED_SHADER(vs_color),
	BGFX_EMBEDDED_SHADER(vs_tex),
	BGFX_EMBEDDED_SHADER(vs_tex_billboard),
	BGFX_EMBEDDED_SHADER(vs_tex_billboard_instanced),
	BGFX_EMBEDDED_SHADER(vs_phong),
	BGFX_EMBEDDED_SHADER(vs_phong_instanced),
	BGFX_EMBEDDED_SHADER(vs_phong_skinned),
	BGFX_EMBEDDED_SHADER(vs_phong_skinned_instanced),
	BGFX_EMBEDDED_SHADER(vs_text),
	BGFX_EMBEDDED_SHADER(vs_skybox),

	BGFX_EMBEDDED_SHADER(fs_color),
	BGFX_EMBEDDED_SHADER(fs_tex),
	BGFX_EMBEDDED_SHADER(fs_phong),
	BGFX_EMBEDDED_SHADER(fs_phong_cubemap),
	BGFX_EMBEDDED_SHADER(fs_text),
	BGFX_EMBEDDED_SHADER(fs_cubemap),

	BGFX_EMBEDDED_SHADER_END()
};


namespace se::gfx
{
	static const char* getVertexShaderName(DefaultShaderType _type)
	{
		switch (_type)
		{
			case DefaultShaderType::Count: break;
			case DefaultShaderType::Color:							return "vs_color";
			case DefaultShaderType::Texture:						return "vs_tex";
			case DefaultShaderType::Texture_Billboard:				return "vs_tex_billboard";
			case DefaultShaderType::Texture_Billboard_Instanced:	return "vs_tex_billboard_instanced";
			case DefaultShaderType::Phong:							return "vs_phong";
			case DefaultShaderType::Phong_Cubemap:					return "vs_phong";
			case DefaultShaderType::Phong_Instanced:				return "vs_phong_instanced";
			case DefaultShaderType::Phong_Skinned:					return "vs_phong_skinned";
			case DefaultShaderType::Phong_Skinned_Instanced:		return "vs_phong_skinned_instanced";
			case DefaultShaderType::Skybox:							return "vs_skybox";
			case DefaultShaderType::Text:							return "vs_text";
		}
		log::error("Unknown DefaultShaderType!");
		return "";
	}
	static const char* getFragmentShaderName(DefaultShaderType _type)
	{
		switch (_type)
		{
			case DefaultShaderType::Count: break;
			case DefaultShaderType::Color:							return "fs_color";
			case DefaultShaderType::Texture:						return "fs_tex";
			case DefaultShaderType::Texture_Billboard:				return "fs_tex";
			case DefaultShaderType::Texture_Billboard_Instanced:	return "fs_tex";
			case DefaultShaderType::Phong:							return "fs_phong";
			case DefaultShaderType::Phong_Cubemap:					return "fs_phong_cubemap";
			case DefaultShaderType::Phong_Instanced:				return "fs_phong";
			case DefaultShaderType::Phong_Skinned:					return "fs_phong";
			case DefaultShaderType::Phong_Skinned_Instanced:		return "fs_phong";
			case DefaultShaderType::Skybox:							return "fs_cubemap";
			case DefaultShaderType::Text:							return "fs_text";
		}
		log::error("Unknown DefaultShaderType!");
		return "";
	}
	const char* getDefaultShaderName(DefaultShaderType _type)
	{
		switch (_type)
		{
			case DefaultShaderType::Count: break;
			case DefaultShaderType::Color:							return "Color";
			case DefaultShaderType::Texture:						return "Tex";
			case DefaultShaderType::Texture_Billboard:				return "Tex_Billboard";
			case DefaultShaderType::Texture_Billboard_Instanced:	return "Tex_Billboard_Instanced";
			case DefaultShaderType::Phong:							return "Phong";
			case DefaultShaderType::Phong_Cubemap:					return "Phong_Cubemap";
			case DefaultShaderType::Phong_Instanced:				return "Phong_Instanced";
			case DefaultShaderType::Phong_Skinned:					return "Phong_Skinned";
			case DefaultShaderType::Phong_Skinned_Instanced:		return "Phong_Skinned_Instanced";
			case DefaultShaderType::Skybox:							return "Skybox";
			case DefaultShaderType::Text:							return "Text";
		}
		log::error("Unknown DefaultShaderType!");
		return "";
	}
	std::shared_ptr<Shader> createDefaultShader(DefaultShaderType _type, AsyncTaskManager* _taskManager)
	{
		const bgfx::RendererType::Enum rendererType = bgfx::getRendererType();
		bgfx::ShaderHandle vertexShader = bgfx::createEmbeddedShader(
			embeddedShaders, rendererType, getVertexShaderName(_type));
		bgfx::ShaderHandle fragmentShader = bgfx::createEmbeddedShader(
			embeddedShaders, rendererType, getFragmentShaderName(_type));

		auto shader = std::make_shared<Shader>(getDefaultShaderName(_type));
		shader->load(_taskManager, AssetHandle{ vertexShader.idx }, AssetHandle{ fragmentShader.idx });
		shader->setPurgeProtection(true);
		return shader;
	}
	void createDefaultShaders(AssetManager& _manager)
	{
		for (DefaultShaderType type : enumRange(DefaultShaderType::Count))
		{
			_manager.push(createDefaultShader(type, _manager.getAsyncTaskManager()));
		}
	}
}
