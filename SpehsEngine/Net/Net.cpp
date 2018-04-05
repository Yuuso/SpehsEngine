#include "SpehsEngine/Net/Net.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"

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

	NetLib::NetLib(const CoreLib& coreLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!coreLib.isValid())
			{
				log::error("Cannot initialize input library, rendering library is invalid.");
				return;
			}

			log::info("Current SpehsEngine input library version: " + getVersion());

			valid = true;
		}
	}

	NetLib::~NetLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool NetLib::isValid()
	{
		return valid;
	}
		
	std::string NetLib::getVersion()
	{
		return version;
	}
}