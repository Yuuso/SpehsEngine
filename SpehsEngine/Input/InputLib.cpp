#include "stdafx.h"
#include "SpehsEngine/Input/InputLib.h"

#include "SDL/SDL.h"


namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	InputLib::InputLib()
	{
		instanceCount++;
		if (!valid)
		{
			log::info("Current SpehsEngine input library version: " + getVersion());

			if (SDL_WasInit(0) == 0 && SDL_Init(0) != 0)
			{
				const std::string error("Unable to initialize SDL: %s", SDL_GetError());
				log::error(error);
				return;
			}
			if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0)
			{
				const std::string error("Unable to initialize SDL events sub system: %s", SDL_GetError());
				log::error(error);
				return;
			}
			if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) != 0)
			{
				const std::string error("Unable to initialize SDL joystick sub system: %s", SDL_GetError());
				log::error(error);
				return;
			}
			if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
			{
				const std::string error("Unable to initialize SDL gamecontroller sub system: %s", SDL_GetError());
				log::error(error);
				return;
			}

			valid = true;
		}
	}

	InputLib::~InputLib()
	{
		if (--instanceCount == 0)
		{
			SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
			SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
			SDL_QuitSubSystem(SDL_INIT_EVENTS);
			SDL_Quit();
			valid = false;
		}
	}

	bool InputLib::isValid()
	{
		return valid;
	}

	std::string InputLib::getVersion()
	{
		return version;
	}
}