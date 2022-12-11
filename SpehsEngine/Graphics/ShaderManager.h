#pragma once

#include "SpehsEngine/Graphics/ResourceManager.h"
#include "SpehsEngine/Graphics/Shader.h"


/*
	LIST OF DEFAULT SHADERS

	- color
	- tex
	- tex_billboard
	- tex_billboard_instanced
	- phong
	- phong_instanced
	- phong_skinned
	- phong_skinned_instanced
	- skybox
	- text
*/


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

			void createDefaultShaders();

		private:

			bool defaultShadersCreated = false;
		};
	}
}
