#pragma once

#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"


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


			void setNormalMatrices(const UniformMatrices& _normal);
			void setPrimitiveColor(const Color& _color);

		private:

			void destroy();

			bgfx::UniformHandle normal;
			bgfx::UniformHandle primitiveColor;
		};
	}
}
