#pragma once

#include "SpehsEngine/Graphics/ShaderManager.h"


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
	- text
*/


namespace se
{
	namespace graphics
	{
		class DefaultShaderManager : public ShaderManager
		{
		public:

			DefaultShaderManager();

		private:

			void createDefaultShaders();
		};
	}
}
