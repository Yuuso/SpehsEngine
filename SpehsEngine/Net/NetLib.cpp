#include "stdafx.h"
#include "SpehsEngine/Net/NetLib.h"

#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"

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

	NetLib::NetLib(const CoreLib& coreLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!coreLib.isValid())
			{
				log::error("Cannot initialize input library, core library is invalid.");
				return;
			}

			log::info("Current SpehsEngine net library version: " + getVersion());

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