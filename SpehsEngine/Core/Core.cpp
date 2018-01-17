
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Core/RNG.h"

#include <algorithm>
#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

namespace spehs
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}
	namespace rng
	{
		extern void initialize();
	}

	CoreLib::CoreLib()
	{
		instanceCount++;
		if (!valid)
		{
			log::info("Current SpehsEngine core library version: " + getVersion());

			//ApplicationData

			log::info("Hardware threads: " + std::to_string(std::thread::hardware_concurrency()));

			//INITIALIZATIONS
			rng::initialize();
			time::initialize();

			//Check available integer widths
			if (sizeof(int8_t) != 1 || sizeof(uint8_t) != 1)
				log::warning("8 bit integer width not available!");
			if (sizeof(int16_t) != 2 || sizeof(uint16_t) != 2)
				log::warning("16 bit integer width not available!");
			if (sizeof(int32_t) != 4 || sizeof(uint32_t) != 4)
				log::warning("32 bit integer width not available!");
			if (sizeof(int64_t) != 8 || sizeof(uint64_t) != 8)
				log::warning("64 bit integer width not available!");

			valid = true;
		}
	}
		
	CoreLib::~CoreLib()
	{
		if (--instanceCount == 0)
		{
			rng::unit();
			spehs::ApplicationData::write();
			valid = false;
		}
	}

	bool CoreLib::isValid()
	{
		return valid;
	}
		
	std::string CoreLib::getVersion()
	{
		return version;
	}
}