#include "stdafx.h"
#include "SpehsEngine/Graphics/ShaderManager.h"

#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "bgfx/embedded_shader.h"


namespace se
{
	namespace graphics
	{
		static const bgfx::EmbeddedShader embeddedShaders[] =
		{
			BGFX_EMBEDDED_SHADER(vs_color),
			BGFX_EMBEDDED_SHADER(fs_color),

			BGFX_EMBEDDED_SHADER_END()
		};

		ShaderManager::ShaderManager()
		{
			const bgfx::RendererType::Enum type = bgfx::getRendererType();

			auto& shader = shaders.emplace_back(std::make_unique<Shader>("color"));
			const bgfx::ProgramHandle program =
				bgfx::createProgram(bgfx::createEmbeddedShader(embeddedShaders, type, "vs_color"),
									bgfx::createEmbeddedShader(embeddedShaders, type, "fs_color"),
									true);
			shader->load(program);


//#			define CREATE_EMBEDDED_SHADER(_name) { \
//				auto& shader = shaders.emplace_back(std::make_unique<Shader>(_name)); \
//				const bgfx::ProgramHandle program = \
//					bgfx::createProgram(bgfx::createEmbeddedShader(embeddedShaders, type, "vs_"#_name), \
//										bgfx::createEmbeddedShader(embeddedShaders, type, "fs_"#_name), \
//										true); \
//				shader->load(program); \
//			}
//
//			CREATE_EMBEDDED_SHADER("color")
		}
		ShaderManager::~ShaderManager()
		{
		}

		const Shader* ShaderManager::find(const std::string_view _name)
		{
			for (auto&& shader : shaders)
			{
				if (shader->getName() == _name)
					return shader.get();
			}
			return nullptr;
		}
	}
}