#include "stdafx.h"
#include "SpehsEngine/Net/AddressUtilityFunctions.h"

#include "boost/asio/ip/host_name.hpp"
#include "boost/asio/io_service.hpp"
#include "boost/asio/ip/tcp.hpp"

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
			steamNetworkingAddress.ParseString(endpoint.address.value.c_str());
			steamNetworkingAddress.SetIPv4(steamNetworkingAddress.GetIPv4(), endpoint.port);
			return steamNetworkingAddress;
		}

		se::net::Endpoint fromSteamNetworkingAddress(const SteamNetworkingIPAddr& steamNetworkingAddress)
		{
			se::net::Endpoint endpoint;
			endpoint.address.value.resize(16);
			steamNetworkingAddress.ToString(endpoint.address.value.data(), endpoint.address.value.size(), false);
			endpoint.port = steamNetworkingAddress.m_port;
			return endpoint;
		}
	}
}
