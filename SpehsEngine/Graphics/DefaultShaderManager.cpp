#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultShaderManager.h"

#include "bgfx/embedded_shader.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong.h"


namespace se
{
	namespace graphics
	{
		static const bgfx::EmbeddedShader embeddedShaders[] =
		{
			BGFX_EMBEDDED_SHADER(vs_color),
			BGFX_EMBEDDED_SHADER(fs_color),
			BGFX_EMBEDDED_SHADER(vs_tex),
			BGFX_EMBEDDED_SHADER(fs_tex),
			BGFX_EMBEDDED_SHADER(vs_phong),
			BGFX_EMBEDDED_SHADER(fs_phong),

			BGFX_EMBEDDED_SHADER_END()
		};


		DefaultShaderManager::DefaultShaderManager()
			: ShaderManager()
		{
			createDefaultShaders();
		}

		void DefaultShaderManager::createDefaultShaders()
		{
			se_assert(shaders.size() == 0);
			numDefaultShaders = 0;

			const bgfx::RendererType::Enum type = bgfx::getRendererType();

			auto createDefaultShader =
				[&](const std::string_view _name)
				{
					std::string vertexShaderName = "vs_";
					vertexShaderName += _name;
					std::string fragmentShaderName = "fs_";
					fragmentShaderName += _name;

					bgfx::ShaderHandle vertexShader = bgfx::createEmbeddedShader(embeddedShaders, type, vertexShaderName.c_str());
					bgfx::ShaderHandle fragmentShader = bgfx::createEmbeddedShader(embeddedShaders, type, fragmentShaderName.c_str());

					std::shared_ptr<Shader>& shader = shaders.emplace_back(std::make_shared<Shader>(_name));
					shader->create(vertexShader, fragmentShader);
					shader->extractUniforms(uniforms);

					numDefaultShaders++;
				};

			createDefaultShader("color");
			createDefaultShader("tex");
			createDefaultShader("phong");
		}

		void DefaultShaderManager::reloadShaders(const size_t _startIndex)
		{
			ShaderManager::reloadShaders(_startIndex + numDefaultShaders);
		}
		void DefaultShaderManager::purgeUnusedShaders(const size_t _startIndex)
		{
			ShaderManager::purgeUnusedShaders(_startIndex + numDefaultShaders);
		}
	}
}
