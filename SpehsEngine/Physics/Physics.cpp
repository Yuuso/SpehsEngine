#include "SpehsEngine/Physics/Physics.h"
#include "SpehsEngine/Math/Math.h"
#include "SpehsEngine/Core/Log.h"

namespace spehs
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	PhysicsLib::PhysicsLib(const MathLib& mathLib)
	{
		instanceCount++;
		if (!valid)
		{
			log::info("Current SpehsEngine physics library version: " + getVersion());

			if (!mathLib.isValid())
			{
				log::error("Cannot initialize physics library, math library is invalid.");
				return;
			}

			valid = true;
		}
	}
		
	PhysicsLib::~PhysicsLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool PhysicsLib::isValid()
	{
		return valid;
	}
		
	std::string PhysicsLib::getVersion()
	{
		return version;
	}
}