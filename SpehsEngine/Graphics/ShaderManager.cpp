#include "stdafx.h"
#include "SpehsEngine/Graphics/ShaderManager.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringViewUtilityFunctions.h"

#include "bgfx/embedded_shader.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex_billboard.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex_billboard_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_skinned.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_skinned_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong_cubemap.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_text.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_text.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_skybox.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_cubemap.h"


namespace se
{
	namespace graphics
	{
		static const bgfx::EmbeddedShader embeddedShaders[] =
		{
			BGFX_EMBEDDED_SHADER(vs_color),
			BGFX_EMBEDDED_SHADER(fs_color),
			BGFX_EMBEDDED_SHADER(vs_tex),
			BGFX_EMBEDDED_SHADER(vs_tex_billboard),
			BGFX_EMBEDDED_SHADER(vs_tex_billboard_instanced),
			BGFX_EMBEDDED_SHADER(fs_tex),
			BGFX_EMBEDDED_SHADER(vs_phong),
			BGFX_EMBEDDED_SHADER(vs_phong_instanced),
			BGFX_EMBEDDED_SHADER(vs_phong_skinned),
			BGFX_EMBEDDED_SHADER(vs_phong_skinned_instanced),
			BGFX_EMBEDDED_SHADER(fs_phong),
			BGFX_EMBEDDED_SHADER(fs_phong_cubemap),
			BGFX_EMBEDDED_SHADER(vs_text),
			BGFX_EMBEDDED_SHADER(fs_text),
			BGFX_EMBEDDED_SHADER(vs_skybox),
			BGFX_EMBEDDED_SHADER(fs_cubemap),

			BGFX_EMBEDDED_SHADER_END()
		};

		std::shared_ptr<Shader> ShaderManager::create(const std::string_view _name,
													  const std::string_view _vertexShader,
													  const std::string_view _fragmentShader)
		{
			const std::string vertexShaderPath = pathFinder->getPath(_vertexShader);
			const std::string fragmentShaderPath = pathFinder->getPath(_fragmentShader);

			for (auto& shader : resources)
			{
				if (shader->getName() == _name)
				{
					log::warning("Cannot create shader '" + _name + "', shader with that name already exists!");
					if (shader->vertexShaderPath != vertexShaderPath || shader->fragmentShaderPath != fragmentShaderPath)
						log::error("Existing shader's '" + _name + "' resource paths don't match!");
					return shader;
				}
			}

			resources.push_back(std::make_shared<Shader>(_name));
			std::shared_ptr<Shader>& shader = resources.back();
			shader->create(vertexShaderPath, fragmentShaderPath, resourceLoader);
			return shader;
		}
		std::shared_ptr<Shader> ShaderManager::find(const std::string_view _name) const
		{
			for (auto& shader : resources)
			{
				if (shader->getName() == _name)
					return shader;
			}
			return nullptr;
		}

		void ShaderManager::createDefaultShaders()
		{
			if (defaultShadersCreated)
			{
				log::warning("Default shaders already created!");
				return;
			}
			defaultShadersCreated = true;

			const bgfx::RendererType::Enum type = bgfx::getRendererType();

			auto createDefaultShader =
				[&](const std::string_view _name, const std::string_view _vertexShader, const std::string_view _fragmentShader)
				{
					std::string vertexShaderName = "vs_";
					vertexShaderName += _vertexShader;
					std::string fragmentShaderName = "fs_";
					fragmentShaderName += _fragmentShader;

					bgfx::ShaderHandle vertexShader = bgfx::createEmbeddedShader(embeddedShaders, type, vertexShaderName.c_str());
					bgfx::ShaderHandle fragmentShader = bgfx::createEmbeddedShader(embeddedShaders, type, fragmentShaderName.c_str());

					resources.push_back(std::make_shared<Shader>(_name));
					resources.back()->create(vertexShader.idx, fragmentShader.idx);
				};

			createDefaultShader("color",					"color",					"color");
			createDefaultShader("tex",						"tex",						"tex");
			createDefaultShader("tex_billboard",			"tex_billboard",			"tex");
			createDefaultShader("tex_billboard_instanced",	"tex_billboard_instanced",	"tex");
			createDefaultShader("phong",					"phong",					"phong");
			createDefaultShader("phong_cubemap",			"phong",					"phong_cubemap");
			createDefaultShader("phong_instanced",			"phong_instanced",			"phong");
			createDefaultShader("phong_skinned",			"phong_skinned",			"phong");
			createDefaultShader("phong_skinned_instanced",	"phong_skinned_instanced",	"phong");
			createDefaultShader("text",						"text",						"text");
			createDefaultShader("skybox",					"skybox",					"cubemap");
		}
	}
}