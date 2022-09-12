#pragma once

#include "SpehsEngine/Net/Endpoint.h"
#include "boost/asio/ip/udp.hpp"


namespace se
{
	namespace net
	{
		Endpoint commandLineArgumentsToEndpoint(const int argc, const char** argv);
		std::string toString(const boost::asio::ip::udp::endpoint& endpoint);
	}
}
