#pragma once

#include "SpehsEngine/Graphics/Shader.h"
#include "SpehsEngine/Graphics/ResourcePathFinder.h"
#include "SpehsEngine/Graphics/Uniform.h"
#include <vector>
#include <memory>
#include <string>


namespace se
{
	namespace graphics
	{
		class ShaderManager
		{
		public:

			ShaderManager();
			virtual ~ShaderManager() = default;

			ShaderManager(const ShaderManager& _other) = delete;
			ShaderManager& operator=(const ShaderManager& _other) = delete;

			ShaderManager(ShaderManager&& _other) = delete;
			ShaderManager& operator=(ShaderManager&& _other) = delete;


			virtual void reloadShaders();
			virtual void purgeUnusedShaders(const size_t _startIndex = 0);

			void setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder);

			std::shared_ptr<Shader> createShader(const std::string_view _name,
												 const std::string_view _vertexShader,
												 const std::string_view _fragmentShader);
			std::shared_ptr<Shader> findShader(const std::string_view _name) const;

			std::shared_ptr<Uniform> findUniform(const std::string_view _name);

		protected:

			std::shared_ptr<ResourcePathFinder> pathFinder;

			std::vector<std::shared_ptr<Shader>> shaders;
			std::vector<std::shared_ptr<Uniform>> uniforms;
		};
	}
}
