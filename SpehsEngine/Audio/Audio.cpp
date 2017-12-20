
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Audio/Audio.h"
#include "SpehsEngine/Audio/AudioManager.h"
#include "SpehsEngine/Audio/AudioEngine.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

namespace spehs
{
	namespace audio
	{
		namespace
		{
			bool initialized = false;
		}

		int initialize()
		{
			_ASSERT(spehs::core::isInitialized() && "Spehs core must be initialized");
			log::info("Current SpehsEngine audio library build: " + getVersion());
			
			//INITIALIZATIONS
			audio::AudioEngine::init();
			AudioManager::init();

			initialized = true;
			return 0;
		}
		
		void uninitialize()
		{
			audio::AudioEngine::uninit();
			AudioManager::uninit();
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