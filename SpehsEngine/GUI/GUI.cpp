#include "stdafx.h"
#include "SpehsEngine/GUI/GUI.h"
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