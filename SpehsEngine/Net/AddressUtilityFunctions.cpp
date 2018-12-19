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
				se::log::warning(error.message());
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
	}
}
