#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include <string>


namespace se
{
	namespace graphics
	{
		class ShaderPathFinder
		{
		public:
			virtual std::string getShaderPath(const RendererBackend /*_rendererBackend*/) const
			{
				return "";
			}
		};
	}
}
