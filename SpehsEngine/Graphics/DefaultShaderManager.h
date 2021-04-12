#pragma once

#include "SpehsEngine/Graphics/ShaderManager.h"


/*
	LIST OF DEFAULT SHADERS

	- color
	- tex
	- tex_billboard
	- phong
	- phong_skinned
	- phong_instanced
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
