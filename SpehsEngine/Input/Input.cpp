#include "stdafx.h"

#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Core/Exceptions.h"
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

			valid = true;
		}
	}

	InputLib::~InputLib()
	{
		if (--instanceCount == 0)
		{
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