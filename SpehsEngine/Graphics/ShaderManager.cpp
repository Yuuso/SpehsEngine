#include "stdafx.h"
#include "SpehsEngine/Graphics/ShaderManager.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringOperations.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Graphics/Renderer.h"

namespace se
{
	namespace graphics
	{
		ShaderManager::ShaderManager()
		{
			shaderPathFinder = std::make_shared<ShaderPathFinder>();
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
		void ShaderManager::purgeUnusedShaders(const size_t _startIndex)
		{
			for (size_t i = _startIndex; i < shaders.size(); /*i++*/)
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
			const std::string shaderPath = shaderPathFinder->getShaderPath(getRendererBackend());
			const std::string vertexShaderPath = shaderPath + _vertexShader;
			const std::string fragmentShaderPath = shaderPath + _fragmentShader;

			for (auto& shader : shaders)
			{
				if (shader->getName() == _name)
				{
					log::warning("Cannot create shader '" + _name + "', shader with that name already exists!");
					se_assert(shader->vertexShaderPath == vertexShaderPath && shader->fragmentShaderPath == fragmentShaderPath);
					return shader;
				}
			}

			std::shared_ptr<Shader>& shader = shaders.emplace_back(std::make_shared<Shader>(_name));
			shader->create(vertexShaderPath, fragmentShaderPath);
			extractUniforms(shader);
			return shader;
		}
		std::shared_ptr<Shader> ShaderManager::findShader(const std::string_view _name) const
		{
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