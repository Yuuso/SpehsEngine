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

			void reloadShaders(const size_t _startIndex = 0) override;
			void purgeUnusedShaders(const size_t _startIndex = 0) override;

		private:

			void createDefaultShaders();

			size_t numDefaultShaders;
		};
	}
}
