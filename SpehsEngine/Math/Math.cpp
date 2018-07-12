#include "SpehsEngine/Math/Math.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Core/Log.h"

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

	MathLib::MathLib(const CoreLib& coreLib)
	{
		instanceCount++;
		if (!valid)
		{
			log::info("Current SpehsEngine math library version: " + getVersion());

			if (!coreLib.isValid())
			{
				log::error("Cannot initialize math library, core library is invalid.");
				return;
			}

			valid = true;
		}
	}
		
	MathLib::~MathLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool MathLib::isValid()
	{
		return valid;
	}
		
	std::string MathLib::getVersion()
	{
		return version;
	}
}