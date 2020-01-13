#include "stdafx.h"
#include "SpehsEngine/Graphics/GraphicsLib.h"

#include "SpehsEngine/Core/Log.h"
#include <SDL/SDL.h>
#include <string>


namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
	}

	GraphicsLib::GraphicsLib()
	{
		instanceCount++;
		if (!valid)
		{
			if (SDL_WasInit(0) == 0 && SDL_Init(0) != 0)
			{
				const std::string error("Unable to initialize SDL: %s", SDL_GetError());
				log::error(error);
				return;
			}
			if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
			{
				std::string error = "Unable to init SDL video sub system: ";
				error += SDL_GetError();
				se::log::error(error);
				return;
			}

			valid = true;
		}
	}

	GraphicsLib::~GraphicsLib()
	{
		if (--instanceCount == 0)
		{
			SDL_QuitSubSystem(SDL_INIT_VIDEO);
			SDL_Quit();
			valid = false;
		}
	}

	bool GraphicsLib::isValid()
	{
		return valid;
	}
}