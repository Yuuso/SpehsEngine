#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultShaderManager.h"

#include "bgfx/embedded_shader.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_text.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_text.h"


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
			BGFX_EMBEDDED_SHADER(vs_text),
			BGFX_EMBEDDED_SHADER(fs_text),

			BGFX_EMBEDDED_SHADER_END()
		};


		DefaultShaderManager::DefaultShaderManager()
			: ShaderManager()
		{
			createDefaultShaders();
		}

		void DefaultShaderManager::createDefaultShaders()
		{
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

					resources.push_back(std::make_shared<Shader>(_name));
					resources.back()->create(vertexShader.idx, fragmentShader.idx);
				};

			createDefaultShader("color");
			createDefaultShader("tex");
			createDefaultShader("phong");
			createDefaultShader("text");
		}
	}
}
