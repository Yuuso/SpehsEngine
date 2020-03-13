#pragma once

#include <bgfx/bgfx.h> // !
#include "SpehsEngine/Graphics/Uniform.h"
#include <string>


namespace se
{
	namespace graphics
	{
		class Shader
		{
		public:

			~Shader();

			Shader(const Shader& _other) = delete;
			Shader& operator=(const Shader& _other) = delete;

			Shader(Shader&& _other) = delete;
			Shader& operator=(Shader&& _other) = delete;


			void reload();

			const std::string& getName() const;

			Shader(const std::string_view _name);

		private:

			friend class PrimitiveInstance;
			friend class Batch;
			friend class ShaderManager;

			void destroy();
			void create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath);
			void create(bgfx::ShaderHandle _vertexShader, bgfx::ShaderHandle _fragmentShader);

			const std::string name;

			std::string vertexShaderPath;
			std::string fragmentShaderPath;

			std::vector<std::shared_ptr<Uniform>> uniforms;

			bgfx::ProgramHandle programHandle;
		};
	}
}
