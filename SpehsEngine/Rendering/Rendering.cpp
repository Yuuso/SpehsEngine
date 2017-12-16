
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Input/Window.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Rendering/Console.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Input/OpenGLError.h"
#include "SpehsEngine/Rendering/Text.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

namespace spehs
{
	extern void initText();
	extern void uninitText();

	namespace rendering
	{
		namespace
		{
			bool initialized = false;
		}

		int initialize()
		{
			log::info("Current SpehsEngine rendering library build: " + getVersion());

			initText();
			console::initialize();

			//INITIALIZATIONS
			textureManager = new TextureManager();
			shaderManager = new ShaderManager();

			checkOpenGLErrors(__FILE__, __LINE__);

			initialized = true;
			return 0;
		}
		
		void uninitialize()
		{
			delete textureManager;
			textureManager = nullptr;
			delete shaderManager;
			shaderManager = nullptr;
			console::unitialize();
			uninitText();
			initialized = false;
		}

		bool isInitialized()
		{
			return initialized;
		}
				
		std::string getVersion()
		{
			return std::string("0");
		}
	}
}