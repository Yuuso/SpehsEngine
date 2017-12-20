
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Input/InputManager.h"
#include "SpehsEngine/Input/Window.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include <boost/bind.hpp>

namespace spehs
{
	namespace input
	{
		namespace
		{
			bool initialized = false;
			Window* mainWindow = nullptr;
		}

		int initialize()
		{
			_ASSERT(spehs::core::isInitialized() && "Spehs core must be initialized");
			log::info("Current SpehsEngine input library version: " + getVersion());
			
			if (SDL_Init(SDL_INIT_VIDEO) < 0)
			{
				exceptions::fatalError("\nVideo initialization failed!");
				return 1;
			}

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

			SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

			//Multisampling
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, spehs::ApplicationData::MSAA);
			
			//Create the main window(?)
			mainWindow = new Window(spehs::ApplicationData::getWindowWidth(), spehs::ApplicationData::getWindowHeight());
			mainWindow->setFullscreen(spehs::ApplicationData::fullscreen);
			
			//INITIALIZATIONS
			inputManager = new InputManager();
			inputManager->initialize();

			initialized = true;
			return 0;
		}

		void uninitialize()
		{
			inputManager->uninitialize();
			delete inputManager;
			inputManager = nullptr;
			delete mainWindow;
			mainWindow = nullptr;
			SDL_Quit();
			initialized = false;
		}

		bool isInitialized()
		{
			return initialized;
		}
		
		Window* getMainWindow()
		{
			return mainWindow;
		}
		
		std::string getVersion()
		{
			return std::string("0");
		}
	}
}