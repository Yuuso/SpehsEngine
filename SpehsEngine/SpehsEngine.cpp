#include "ApplicationData.h"
#include "SpehsEngine.h"
#include "Exceptions.h"
#include "Text.h"
#include "Time.h"
#include "Window.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "RNG.h"
#include "ShaderManager.h"
#include "ModelManager.h"
#include "Console.h"
#include "OpenGLError.h"

#include <string>
#include <thread>
#include <iostream>

#include <SDL/SDL.h>
#include <GL/glew.h>



namespace spehs
{
	int initialize(std::string _windowName, ApplicationData* customApplicationDataInstance)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			exceptions::fatalError("\nVideo initialization failed!");
			return 1;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

		//ApplicationData
		if (customApplicationDataInstance)
		{//Set application data pointer to custom instance
			applicationData = customApplicationDataInstance;
		}
		else
		{//Create a basic application data instance
			applicationData = new ApplicationData();
		}
		applicationData->read();//Load application data

		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

		//Multisampling
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, applicationData->MSAA);

		mainWindow = new spehs::Window();
		mainWindow->create(_windowName, applicationData->getWindowWidth(), applicationData->getWindowHeight(), applicationData->windowMode);//To get e.g. Fullscreen mode - replace '0' with 'spehs::FULLSCREEN'

		std::cout << "\nHardware threads: " + std::to_string(std::thread::hardware_concurrency());

		spehs::initializeTextRendering();
		spehs::initializeTime();

		//INITIALIZATIONS
		inputManager = new InputManager();
		inputManager->initialize();
		textureManager = new TextureManager();
		modelManager = new ModelManager();
		rng = new RNG();
		shaderManager = new ShaderManager();
		console::initialize();

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif

		return 0;
	}


	void uninitialize()
	{
		applicationData->write();
		delete applicationData;
		inputManager->uninitialize();
		delete inputManager;
		delete textureManager;
		delete rng;
		delete shaderManager;
		delete modelManager;
		console::unitialize();
		spehs::uninitializeTime();
		spehs::uninitializeTextRendering();
		SDL_Quit();
	}
}