#include "SpehsEngine/Game/Game.h"
#include "SpehsEngine/Core/Core.h"
#include "SpehsEngine/Input/Input.h"
#include "SpehsEngine/Rendering/Rendering.h"
#include "SpehsEngine/Audio/Audio.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Time.h"

namespace spehs
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