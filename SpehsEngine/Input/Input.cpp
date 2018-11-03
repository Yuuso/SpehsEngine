#include "stdafx.h"

#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Rendering/Window.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <boost/bind.hpp>

namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	InputLib::InputLib(const se::rendering::RenderingLib& renderingLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!renderingLib.isValid())
			{
				log::error("Cannot initialize input library, rendering library is invalid.");
				return;
			}

			log::info("Current SpehsEngine input library version: " + getVersion());

			if (SDL_Init(0) != 0)
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

	void InputLib::update()
	{
		SDL_PumpEvents();
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