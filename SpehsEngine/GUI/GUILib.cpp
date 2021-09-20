#include "stdafx.h"
#include "SpehsEngine/GUI/GUILib.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Audio/AudioLib.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"

namespace se
{
	namespace
	{
		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	GUILib::GUILib(const InputLib& inputLib, const AudioLib& audioLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!inputLib.isValid())
			{
				log::error("Cannot initialize gui library, input library is invalid.");
				return;
			}
			if (!audioLib.isValid())
			{
				log::error("Cannot initialize gui library, audio library is invalid.");
				return;
			}

			log::info("Current SpehsEngine gui library build: " + getVersion());
			valid = true;
		}
	}

	GUILib::~GUILib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool GUILib::isValid()
	{
		return valid;
	}

	std::string GUILib::getVersion()
	{
		return version;
	}
}