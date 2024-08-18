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
					return Address(endpoint.address().to_v4().to_uint());
				}
				it++;
			}
			return Address();
		}

		SteamNetworkingIPAddr toSteamNetworkingAddress(const se::net::Endpoint& endpoint)
		{
			SteamNetworkingIPAddr steamNetworkingAddress;
			steamNetworkingAddress.Clear();
			if (!SteamNetworkingUtils()->SteamNetworkingIPAddr_ParseString(&steamNetworkingAddress, endpoint.address.toString().c_str()))
			{
				se::log::warning("Failed to parse endpoint address");
			}
			steamNetworkingAddress.SetIPv4(steamNetworkingAddress.GetIPv4(), endpoint.port);
			return steamNetworkingAddress;
		}

		se::net::Endpoint fromSteamNetworkingAddress(const SteamNetworkingIPAddr& steamNetworkingAddress)
		{
			se::net::Endpoint endpoint;
			if (steamNetworkingAddress.IsIPv4())
			{
				endpoint.address = Address(steamNetworkingAddress.GetIPv4());
			}
			else
			{
				endpoint.address = Address(&steamNetworkingAddress.m_ipv6[0]);
			}
			endpoint.port = steamNetworkingAddress.m_port;
			return endpoint;
		}

		bool isValidEmailAddress(const std::string_view string)
		{
			if (string.empty())
			{
				return false;
			}
			auto isCharacter = [](char c)
			{
				return ((c >= 'a' && c <= 'z')
					|| (c >= 'A' && c <= 'Z'));
			};
			if (!isCharacter(string[0]))
			{
				return false; // Must start with a character
			}
			auto isDigit = [](const char c)
			{
				return (c >= '0' && c <= '9');
			};
			size_t atCount = 0;
			size_t postAtDotCount = 0;
			bool domain1 = false;
			bool domain2 = false;
			for (size_t i = 0; i < string.size(); i++)
			{
				if (string[i] == '@')
				{
					atCount++;
				}
				else if (string[i] == '.')
				{
					if (atCount > 0)
					{
						postAtDotCount++;
					}
				}
				else if (isCharacter(string[i]) || isDigit(string[i]))
				{
					if (atCount > 0)
					{
						if (postAtDotCount == 0)
						{
							domain1 = true;
						}
						else
						{
							domain2 = true;
						}
					}
				}
				else
				{
					return false;
				}
			}
			return atCount == 1 && postAtDotCount == 1 && domain1 && domain2;
		}
	}
}
