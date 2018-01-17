
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Rendering/Console.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Rendering/Text.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

namespace spehs
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	RenderingLib::RenderingLib(const CoreLib& coreLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!coreLib.isValid())
			{
				log::error("Cannot initialize rendering library, core library is invalid.");
				return;
			}

			log::info("Current SpehsEngine rendering library build: " + getVersion());

			if (SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				spehs::log::info(SDL_GetError());
				spehs::log::error("Video initialization failed!");
				return;
			}

			valid = true;
		}
	}
		
	RenderingLib::~RenderingLib()
	{
		if (--instanceCount == 0)
		{
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