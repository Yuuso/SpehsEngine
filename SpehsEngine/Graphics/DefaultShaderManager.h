#pragma once

#include "SpehsEngine/Graphics/ShaderManager.h"


/*
	LIST OF DEFAULT SHADERS

	- color
	- tex
	- phong
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
