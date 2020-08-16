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

			Shader() = delete;
			Shader(const std::string_view _name);
			~Shader();

			Shader(const Shader& _other) = delete;
			Shader& operator=(const Shader& _other) = delete;

			Shader(Shader&& _other) = delete;
			Shader& operator=(Shader&& _other) = delete;


			void reload();

			const std::string& getName() const;

		private:

			friend class PrimitiveInstance;
			friend class Batch;
			friend class ShaderManager;
			friend class DefaultShaderManager;

			void destroy();
			void create(const std::string_view _vertexShaderPath, const std::string_view _fragmentShaderPath);
			void create(bgfx::ShaderHandle _vertexShader, bgfx::ShaderHandle _fragmentShader);

			const std::string name;

			std::string vertexShaderPath;
			std::string fragmentShaderPath;

			bgfx::ProgramHandle programHandle;
		};
	}
}
