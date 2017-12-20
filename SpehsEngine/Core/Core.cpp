
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
	namespace rng
	{
		extern void initialize();
	}

	namespace core
	{

		namespace
		{
			bool initialized = false;
		}

		int initialize()
		{
			log::info("Current SpehsEngine core library version: " + getVersion());

			//ApplicationData
			spehs::ApplicationData::read();

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
			
			initialized = true;
			return 0;
		}
		
		void uninitialize()
		{
			rng::unit();
			spehs::ApplicationData::write();
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