#include "stdafx.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Audio/AudioLib.h"
#include "SpehsEngine/Audio/AudioManager.h"
#include "SpehsEngine/Audio/AudioEngine.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	AudioLib::AudioLib(const CoreLib& coreLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!coreLib.isValid())
			{
				log::error("Cannot initialize audio library, core library is invalid.");
				return;
			}

			log::info("Current SpehsEngine audio library build: " + getVersion());

			//INITIALIZATIONS
			audio::AudioEngine::init();
			AudioManager::init();

			valid = true;
		}
	}
		
	AudioLib::~AudioLib()
	{
		if (--instanceCount == 0)
		{
			audio::AudioEngine::uninit();
			AudioManager::uninit();
			valid = false;
		}
	}

	bool AudioLib::isValid()
	{
		return valid;
	}

	std::string AudioLib::getVersion()
	{
		return version;
	}
}