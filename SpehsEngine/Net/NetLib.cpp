#include "stdafx.h"
#include "SpehsEngine/Net/NetLib.h"

#include "SpehsEngine/Core/CoreLib.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "steam/steamnetworkingsockets.h"
#include "steam/isteamnetworkingutils.h"
#include <string>
#include <thread>
#include <iostream>
#include <stdint.h>

namespace se
{
	namespace
	{
		void steamNetworkingDebug(ESteamNetworkingSocketsDebugOutputType debugOutputType, const char* message)
		{
			se::log::info(message);
		}

		int instanceCount = 0;
		bool valid = false;
		std::string version("0");
	}

	NetLib::NetLib(const CoreLib& coreLib)
	{
		instanceCount++;
		if (!valid)
		{
			if (!coreLib.isValid())
			{
				log::error("Cannot initialize input library, core library is invalid.");
				return;
			}

			log::info("Current SpehsEngine net library version: " + getVersion());

			SteamDatagramErrMsg errorMessage;
			if (!GameNetworkingSockets_Init(nullptr, errorMessage))
			{
				se::log::error(se::formatString("GameNetworkingSockets_Init failed.  %s", errorMessage));
				return;
			}
			SteamNetworkingUtils()->SetDebugOutputFunction(k_ESteamNetworkingSocketsDebugOutputType_Msg, steamNetworkingDebug);

			valid = true;
		}
	}

	NetLib::~NetLib()
	{
		if (--instanceCount == 0)
		{
			// Give connections time to finish up. This is an application layer protocol here, it's not TCP. Note that if you have an application and you need to be more sure about cleanup, you won't be able to do this.  You will need to send a message and then either wait for the peer to close the connection, or you can pool the connection to see if any reliable data is pending.
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			GameNetworkingSockets_Kill();

			valid = false;
		}
	}

	bool NetLib::isValid()
	{
		return valid;
	}
		
	std::string NetLib::getVersion()
	{
		return version;
	}
}