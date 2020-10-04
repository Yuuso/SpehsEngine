#pragma once

#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>


/*
	This is used to internally set some default uniform values by the engine.
	Default uniforms are also listed in Uniform.h and se_shader.sh.
*/

namespace se
{
	namespace graphics
	{
		class DefaultUniforms
		{
		public:

			DefaultUniforms();
			~DefaultUniforms();

			DefaultUniforms(const DefaultUniforms& _other) = delete;
			DefaultUniforms& operator=(const DefaultUniforms& _other) = delete;

			DefaultUniforms(DefaultUniforms&& _other) = delete;
			DefaultUniforms& operator=(DefaultUniforms&& _other) = delete;


			void setNormalMatrix(const glm::mat4& _normal);

		private:

			void destroy();

			bgfx::UniformHandle normal;
		};
	}
}
