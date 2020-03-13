#include "stdafx.h"
#include "SpehsEngine/Graphics/ShaderManager.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_color.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/vs_phong.h"
#include "SpehsEngine/Graphics/EmbeddedShaders/fs_phong.h"
#include "SpehsEngine/Graphics/Renderer.h"
#include "bgfx/embedded_shader.h"


namespace se
{
	namespace graphics
	{
		static const bgfx::EmbeddedShader embeddedShaders[] =
		{
			BGFX_EMBEDDED_SHADER(vs_color),
			BGFX_EMBEDDED_SHADER(fs_color),
			BGFX_EMBEDDED_SHADER(vs_phong),
			BGFX_EMBEDDED_SHADER(fs_phong),

			BGFX_EMBEDDED_SHADER_END()
		};

		ShaderManager::ShaderManager()
		{
			createDefaultShaders();
		}
		ShaderManager::~ShaderManager()
		{
			uniforms.clear();
			shaders.clear();
			defaultShaders.clear();
		}

		void ShaderManager::createDefaultShaders()
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

					std::shared_ptr<Shader>& shader = defaultShaders.emplace_back(std::make_shared<Shader>(_name));
					shader->create(vertexShader, fragmentShader);
					extractUniforms(shader);
				};

			createDefaultShader("color");
			createDefaultShader("phong");
		}
		void ShaderManager::extractUniforms(std::shared_ptr<Shader>& _shader)
		{
			for (auto& shaderUniform : _shader->uniforms)
			{
				if (std::find_if(uniforms.begin(), uniforms.end(), [shaderUniform](const std::shared_ptr<Uniform>& _uniform){ return *_uniform == *shaderUniform; }) == uniforms.end())
				{
					continue;
				}
				uniforms.push_back(shaderUniform);
			}
		}


		void ShaderManager::reloadShaders()
		{
			for (auto&& shader : shaders)
			{
				shader->reload();
			}
		}
		void ShaderManager::purgeUnusedShaders()
		{
			for (size_t i = 0; i < shaders.size(); /*i++*/)
			{
				if (shaders[i].use_count() == 1)
				{
					shaders[i].swap(shaders.back());
					shaders.pop_back();
				}
				else
				{
					i++;
				}
			}

			for (size_t i = 0; i < uniforms.size(); /*i++*/)
			{
				if (uniforms[i].use_count() == 1)
				{
					uniforms[i].swap(uniforms.back());
					uniforms.pop_back();
				}
				else
				{
					i++;
				}
			}
		}

		void ShaderManager::setShaderPathFinder(std::shared_ptr<ShaderPathFinder> _shaderPathFinder)
		{
			shaderPathFinder = _shaderPathFinder;
		}

		std::shared_ptr<Shader> ShaderManager::createShader(const std::string_view _name,
															const std::string_view _vertexShader,
															const std::string_view _fragmentShader)
		{
			std::shared_ptr<Shader>& shader = defaultShaders.emplace_back(std::make_shared<Shader>(_name));
			shader->create(shaderPathFinder->getShaderPath(Renderer::getRendererBackend()) + _vertexShader,
						   shaderPathFinder->getShaderPath(Renderer::getRendererBackend()) + _vertexShader);
			extractUniforms(shader);
			return shader;
		}
		std::shared_ptr<Shader> ShaderManager::findShader(const std::string_view _name) const
		{
			for (auto& shader : defaultShaders)
			{
				if (shader->getName() == _name)
					return shader;
			}
			for (auto& shader : shaders)
			{
				if (shader->getName() == _name)
					return shader;
			}
			se_assert_m(false, "Shader '" + std::string(_name) + "' not found!");
			return nullptr;
		}

		std::shared_ptr<Uniform> ShaderManager::findUniform(const std::string_view _name) const
		{
			for (auto& uniform : uniforms)
			{
				if (uniform->getName() == _name)
					return uniform;
			}
			se_assert_m(false, "Uniform '" + std::string(_name) + "' not found!");
			return nullptr;
		}
	}
}