#include "stdafx.h"
#include "SpehsEngine/Graphics/ShaderManager.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Graphics/Renderer.h"

namespace se
{
	namespace graphics
	{
		ShaderManager::ShaderManager()
		{
			pathFinder = std::make_shared<ResourcePathFinder>();
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

		void ShaderManager::setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder)
		{
			pathFinder = _pathFinder;
		}

		std::shared_ptr<Shader> ShaderManager::createShader(const std::string_view _name,
															const std::string_view _vertexShader,
															const std::string_view _fragmentShader)
		{
			const std::string vertexShaderPath = pathFinder->getPath(_vertexShader);
			const std::string fragmentShaderPath = pathFinder->getPath(_fragmentShader);

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
			shader->extractUniforms(uniforms);
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

		std::shared_ptr<Uniform> ShaderManager::findUniform(const std::string_view _name)
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