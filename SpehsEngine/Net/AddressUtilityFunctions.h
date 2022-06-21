#pragma once

#include "SpehsEngine/Net/Address.h"
#include "steam/steamnetworkingtypes.h"


namespace se
{
	namespace net
	{
		Address getLocalAddress();
		SteamNetworkingIPAddr toSteamNetworkingAddress(const se::net::Endpoint& endpoint);
		se::net::Endpoint fromSteamNetworkingAddress(const SteamNetworkingIPAddr& steamNetworkingAddress);
		bool isValidEmailAddress(const std::string_view string);
	}
}
