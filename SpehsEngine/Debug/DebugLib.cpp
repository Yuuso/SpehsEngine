#include "stdafx.h"
#include "SpehsEngine/Debug/DebugLib.h"


namespace se
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
