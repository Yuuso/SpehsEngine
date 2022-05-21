#include "stdafx.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"

#include "boost/asio/ip/host_name.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/tcp.hpp"
#include "steam/isteamnetworkingutils.h"

namespace se
{
	namespace net
	{
		Address getLocalAddress()
		{
			std::string localHostName = boost::asio::ip::host_name();
			boost::asio::io_service io;
			boost::system::error_code error;
			boost::asio::ip::tcp::resolver resolver(io);
			boost::asio::ip::tcp::resolver::query queryTCP(localHostName, "");
			boost::asio::ip::tcp::resolver::iterator end, it = resolver.resolve(queryTCP, error);
			if (error)
			{
				se::log::warning("se::net::getLocalAddress(): failed to resolve local address. Boost error" + error.message());
				return Address();
			}
			while (it != end)
			{
				boost::asio::ip::tcp::endpoint endpoint = *it;
				if (endpoint.address().is_v4())
				{
					return Address(endpoint.address().to_string());
				}
				it++;
			}
			return Address();
		}

		SteamNetworkingIPAddr toSteamNetworkingAddress(const se::net::Endpoint& endpoint)
		{
			SteamNetworkingIPAddr steamNetworkingAddress;
			steamNetworkingAddress.Clear();
			if (!SteamNetworkingUtils()->SteamNetworkingIPAddr_ParseString(&steamNetworkingAddress, endpoint.address.value.c_str()))
			{
				se::log::warning("Failed to parse endpoint address");
			}
			steamNetworkingAddress.SetIPv4(steamNetworkingAddress.GetIPv4(), endpoint.port);
			return steamNetworkingAddress;
		}

		se::net::Endpoint fromSteamNetworkingAddress(const SteamNetworkingIPAddr& steamNetworkingAddress)
		{
			se::net::Endpoint endpoint;
			endpoint.address.value.resize(64);
			SteamNetworkingUtils()->SteamNetworkingIPAddr_ToString(steamNetworkingAddress, endpoint.address.value.data(), endpoint.address.value.size(), false);
			const size_t length = strlen(endpoint.address.value.data());
			endpoint.address.value.resize(length);
			endpoint.port = steamNetworkingAddress.m_port;
			return endpoint;
		}
	}
}
