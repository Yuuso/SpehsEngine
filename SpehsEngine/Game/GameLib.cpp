#include "stdafx.h"
#include "SpehsEngine/Game/GameLib.h"
#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Input/InputLib.h"
#include "SpehsEngine/Rendering/RenderingLib.h"
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

	GameLib::GameLib(const InputLib& inputLib, const AudioLib& audioLib)
	{
		if (instanceCount++ == 0)
		{
			if (!inputLib.isValid())
			{
				log::error("Cannot initialize game library, input library is invalid.");
				return;
			}
			if (!audioLib.isValid())
			{
				log::error("Cannot initialize game library, audio library is invalid.");
				return;
			}

			log::info("Current SpehsEngine game library build: " + getVersion());

			valid = true;
		}
	}
		
	GameLib::~GameLib()
	{
		if (--instanceCount == 0)
		{
			valid = false;
		}
	}

	bool GameLib::isValid()
	{
		return valid;
	}

	std::string GameLib::getVersion()
	{
		return version;
	}
}