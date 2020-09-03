#pragma once

#include "SpehsEngine/Graphics/Internal/ResourceManager.h"
#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/ResourcePathFinder.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include <vector>
#include <memory>
#include <string>


namespace se
{
	namespace graphics
	{
		class ShaderManager : public ResourceManager<Shader>
		{
		public:

			ShaderManager() = default;
			virtual ~ShaderManager() = default;

			ShaderManager(const ShaderManager& _other) = delete;
			ShaderManager& operator=(const ShaderManager& _other) = delete;

			ShaderManager(ShaderManager&& _other) = delete;
			ShaderManager& operator=(ShaderManager&& _other) = delete;


			std::shared_ptr<Shader> create(const std::string_view _name,
										   const std::string_view _vertexShader,
										   const std::string_view _fragmentShader);
			std::shared_ptr<Shader> find(const std::string_view _name) const;
		};
	}
}
