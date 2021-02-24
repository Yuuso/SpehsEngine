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
					se_assert(shader->vertexShaderPath == vertexShaderPath && shader->fragmentShaderPath == fragmentShaderPath);
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
			se_assert_m(false, "Shader '" + std::string(_name) + "' not found!");
			return nullptr;
		}
	}
}