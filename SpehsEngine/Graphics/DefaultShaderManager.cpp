#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultShaderManager.h"

#include "bgfx/embedded_shader.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_tex_billboard.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_tex.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_instanced.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_skinned.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong_skinned_instanced.h"
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
			BGFX_EMBEDDED_SHADER(vs_tex_billboard),
			BGFX_EMBEDDED_SHADER(fs_tex),
			BGFX_EMBEDDED_SHADER(vs_phong),
			BGFX_EMBEDDED_SHADER(vs_phong_instanced),
			BGFX_EMBEDDED_SHADER(vs_phong_skinned),
			BGFX_EMBEDDED_SHADER(vs_phong_skinned_instanced),
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
			createDefaultShader("phong",					"phong",					"phong");
			createDefaultShader("phong_instanced",			"phong_instanced",			"phong");
			createDefaultShader("phong_skinned",			"phong_skinned",			"phong");
			createDefaultShader("phong_skinned_instanced",	"phong_skinned_instanced",	"phong");
			createDefaultShader("text",						"text",						"text");
		}
	}
}
