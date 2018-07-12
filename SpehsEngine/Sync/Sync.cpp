#include "SpehsEngine/Sync/Sync.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"
#include "SpehsEngine/Net/Net.h"

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

	SyncLib::SyncLib(const NetLib& netLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!netLib.isValid())
			{
				log::error("Cannot initialize sync library, rendering library is invalid.");
				return;
			}

			log::info("Current SpehsEngine sync library version: " + getVersion());

			valid = true;
		}
	}

	SyncLib::~SyncLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool SyncLib::isValid()
	{
		return valid;
	}
		
	std::string SyncLib::getVersion()
	{
		return version;
	}
}