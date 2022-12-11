#pragma once

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
			void setBillboardInfo(const glm::vec3& _position, const bool _cylindrical);
			void updateEngineUtility();

		private:

			void destroy();

			bgfx::UniformHandle normal;
			bgfx::UniformHandle primitiveColor;
			bgfx::UniformHandle billboardInfo;
			bgfx::UniformHandle engineUtility;
		};
	}
}
