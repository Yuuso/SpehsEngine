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
			boost::asio::ip::resolver_base::flags flags = boost::asio::ip::resolver_base::flags::all_matching;
			boost::asio::ip::tcp::resolver::query queryTCP(localHostName, "", flags);
			boost::asio::ip::tcp::resolver::iterator end, it = resolver.resolve(queryTCP, error);
			if (error)
			{
				log::warning("se::net::getLocalAddress(): failed to resolve local address. Boost error" + error.message());
				return Address();
			}
			Address result;
			while (it != end)
			{
				const boost::asio::ip::tcp::endpoint endpoint = *it;
				const boost::asio::ip::address address = endpoint.address();
				if (address.is_v4())
				{
					const std::string addressString = address.to_v4().to_string();
					result = Address::makeFromStringIpv4(addressString);

					// If there are multiple addresses then use the (first) one in the default address range
					// Don't know how this should be handled but this works for my current setup
					constexpr std::string defaultAddressRangeStartString = "192.168.";
					if (doesStartWith(addressString, defaultAddressRangeStartString))
					{
						break;
					}
				}
				it++;
			}
			return result;
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
