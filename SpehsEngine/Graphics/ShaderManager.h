#pragma once

#include "SpehsEngine/Graphics/Shader.h"
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
			~ShaderManager();

			ShaderManager(const ShaderManager& _other) = delete;
			ShaderManager& operator=(const ShaderManager& _other) = delete;

			ShaderManager(ShaderManager&& _other) = delete;
			ShaderManager& operator=(ShaderManager&& _other) = delete;


			const Shader* find(const std::string_view _name);

		private:

			std::vector<std::unique_ptr<Shader>> shaders;
		};
	}
}
