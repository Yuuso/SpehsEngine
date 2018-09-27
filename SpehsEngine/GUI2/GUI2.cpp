#include "stdafx.h"
#include "SpehsEngine/GUI2/GUI2.h"
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

	GUI2Lib::GUI2Lib(const InputLib& inputLib, const AudioLib& audioLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!inputLib.isValid())
			{
				log::error("Cannot initialize gui2 library, input library is invalid.");
				return;
			}
			if (!audioLib.isValid())
			{
				log::error("Cannot initialize gui2 library, audio library is invalid.");
				return;
			}

			log::info("Current SpehsEngine gui2 library build: " + getVersion());
			valid = true;
		}
	}

	GUI2Lib::~GUI2Lib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool GUI2Lib::isValid()
	{
		return valid;
	}

	std::string GUI2Lib::getVersion()
	{
		return version;
	}
}