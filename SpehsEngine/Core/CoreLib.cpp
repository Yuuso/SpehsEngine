#include "stdafx.h"
#include "SpehsEngine/Core/CoreLib.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"
#include <string>
#include <thread>


namespace se
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
			// INITIALIZATIONS
			time::initialize();
			rng::initialize();

			// Check available integer widths
			static_assert(sizeof(int8_t) == 1 && sizeof(uint8_t) == 1, "8 bit integer width not available!");
			static_assert(sizeof(int16_t) == 2 && sizeof(uint16_t) == 2, "16 bit integer width not available!");
			static_assert(sizeof(int32_t) == 4 && sizeof(uint32_t) == 4, "32 bit integer width not available!");
			static_assert(sizeof(int64_t) == 8 && sizeof(uint64_t) == 8, "64 bit integer width not available!");

			log::info("Current SpehsEngine core library version: " + getVersion());

			log::info("Hardware threads: " + std::to_string(std::thread::hardware_concurrency()));
			log::info("Size of size_t: " + std::to_string(sizeof(size_t)) + " bytes");
			log::info("Size of void*: " + std::to_string(sizeof(void*)) + " bytes");

			valid = true;
		}
	}

	CoreLib::~CoreLib()
	{
		if (--instanceCount == 0)
		{
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