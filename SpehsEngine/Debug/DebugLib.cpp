#include "stdafx.h"
#include "SpehsEngine/Debug/DebugLib.h"

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <string>
#include <thread>


namespace se
{
	namespace debug
	{
		namespace
		{
			int instanceCount = 0;
			bool valid = false;
			std::string version("0");
		}

		DebugLib::DebugLib()
		{
			instanceCount++;
			if (!valid)
			{
				log::info("Current SpehsEngine debug library version: " + getVersion());

				// INITIALIZATIONS

				valid = true;
			}
		}

		DebugLib::~DebugLib()
		{
			if (--instanceCount == 0)
			{
				valid = false;
			}
		}

		bool DebugLib::isValid()
		{
			return valid;
		}

		std::string DebugLib::getVersion()
		{
			return version;
		}
	}
}
