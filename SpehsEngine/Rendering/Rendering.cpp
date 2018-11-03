#include "stdafx.h"

#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Math/Math.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

namespace se
{
	namespace rendering
	{
		namespace
		{
			int instanceCount = 0;
			bool valid = false;
			std::string version("0");
		}

		RenderingLib::RenderingLib(const MathLib& mathLib)
		{
			instanceCount++;
			if (!valid)
			{
				if (!mathLib.isValid())
				{
					log::error("Cannot initialize rendering library, math library is invalid.");
					return;
				}

				log::info("Current SpehsEngine rendering library build: " + getVersion());

				if (SDL_Init(0) != 0)
				{
					const std::string error("Unable to initialize SDL: %s", SDL_GetError());
					log::error(error);
					return;
				}
				if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
				{
					const std::string error("Unable to initialize SDL video sub system: %s", SDL_GetError());
					log::error(error);
					return;
				}

				valid = true;
			}
		}

		RenderingLib::~RenderingLib()
		{
			if (--instanceCount == 0)
			{
				SDL_QuitSubSystem(SDL_INIT_VIDEO);
				SDL_Quit();
				valid = false;
			}
		}

		bool RenderingLib::isValid()
		{
			return valid;
		}

		std::string RenderingLib::getVersion()
		{
			return version;
		}
	}
}