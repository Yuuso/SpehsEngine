#pragma once

#include "bgfx/bgfx.h" // TODO: No bgfx includes in headers!


namespace se
{
	namespace graphics
	{
		class Shader
		{
		public:

			Shader(const std::string_view _name);
			~Shader();

			Shader(const Shader& _other) = delete;
			Shader& operator=(const Shader& _other) = delete;

			Shader(Shader&& _other) = delete;
			Shader& operator=(Shader&& _other) = delete;


			bool load(const std::string_view _vertexShader, const std::string_view _fragmentShader);
			bool load(const bgfx::ProgramHandle _programHandle);

			const std::string& getName() const;

		//private:
			bgfx::ProgramHandle programHandle;
			const std::string name;
		};
	}
}
