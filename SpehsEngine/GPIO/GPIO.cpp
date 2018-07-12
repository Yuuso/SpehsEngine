#include "stdafx.h"

#include "SpehsEngine/GPIO/GPIO.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Sync/Sync.h"

#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

#include <bcm2835.h>

namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	GPIOLib::GPIOLib(const SyncLib& syncLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!syncLib.isValid())
			{
				log::error("Cannot initialize gpio library, rendering library is invalid.");
				return;
			}

			log::info("Current SpehsEngine GPIO library version: " + getVersion());

#ifdef SPEHS_GPIO
			if (bcm2835_init())
			{
				se::log::info("bcm2835 library successfully initialized.");
			}
			else
			{
				se::log::error("Failed to initialize the bcm 2835 library!");
				return;
			}
#else
			log::warning("SPEHS_GPIO hasn't been defined. GPIO pin control will not be available.");
#endif

			valid = true;
		}
	}

	GPIOLib::~GPIOLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool GPIOLib::isValid()
	{
		return valid;
	}
		
	std::string GPIOLib::getVersion()
	{
		return version;
	}
}