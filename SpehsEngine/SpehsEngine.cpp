
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
#include "Console.h"
#include "AudioManager.h"
#include "AudioEngine.h"
#include "OpenGLError.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <SDL/SDL.h>
#include <GL/glew.h>

//[MR].[mR].[B].[A].[pA]
#define CURRENT_BUILD "0.0.0.0.0."
std::string buildVersion;


spehs::Window* mainWindow = nullptr;
namespace spehs
{
	extern void initText();
	extern void uninitText();
	namespace rng
	{
		extern void initializeRNG();
	}

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
		mainWindow->create(_windowName, applicationData->getWindowWidth(), applicationData->getWindowHeight(), applicationData->windowMode);

		std::cout << "\nHardware threads: " + std::to_string(std::thread::hardware_concurrency());
		std::cout << "\nCurrent SpehsEngine build: " << getEngineVersion() << std::endl;

		initText();
		console::initialize();

		//INITIALIZATIONS
		inputManager = new InputManager();
		inputManager->initialize();
		textureManager = new TextureManager();
		shaderManager = new ShaderManager();
		AudioManager::instance = new AudioManager;
		rng::initializeRNG();
		audio::AudioEngine::init();

		checkOpenGLErrors(__FILE__, __LINE__);

		//Check available integer widths
		if (sizeof(int8_t) != 1 || sizeof(uint8_t) != 1)
			spehs::console::warning("8 bit integer width not available!");
		if (sizeof(int16_t) != 2 || sizeof(uint16_t) != 2)
			spehs::console::warning("16 bit integer width not available!");
		if (sizeof(int32_t) != 4 || sizeof(uint32_t) != 4)
			spehs::console::warning("32 bit integer width not available!");
		if (sizeof(int64_t) != 8 || sizeof(uint64_t) != 8)
			spehs::console::warning("64 bit integer width not available!");

		return 0;
	}


	void uninitialize()
	{
		applicationData->write();
		delete applicationData;
		inputManager->uninitialize();
		delete inputManager;
		delete textureManager;
		delete shaderManager;
		delete AudioManager::instance;
		console::unitialize();
		audio::AudioEngine::uninit();
		uninitText();
		SDL_Quit();
	}


	spehs::Window* getMainWindow()
	{
		return mainWindow;
	}


	std::string getEngineVersion()
	{
		std::string build(CURRENT_BUILD + time::engineBuildYear() + time::engineBuildMonth() + time::engineBuildDay() + time::engineBuildHour() + time::engineBuildMinute() + time::engineBuildSecond());
		return build;
	}
}