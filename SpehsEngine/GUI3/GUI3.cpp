#include "stdafx.h"
#include "SpehsEngine/GUI3/GUI3.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Audio/Audio.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"

namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	GUI3Lib::GUI3Lib(const InputLib& inputLib, const AudioLib& audioLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!inputLib.isValid())
			{
				log::error("Cannot initialize gui3 library, input library is invalid.");
				return;
			}
			if (!audioLib.isValid())
			{
				log::error("Cannot initialize gui3 library, audio library is invalid.");
				return;
			}

			log::info("Current SpehsEngine gui3 library build: " + getVersion());
			valid = true;
		}
	}

	GUI3Lib::~GUI3Lib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool GUI3Lib::isValid()
	{
		return valid;
	}

	std::string GUI3Lib::getVersion()
	{
		return version;
	}
}