#include "stdafx.h"
#include "SpehsEngine/Net/EndpointUtilityFunctions.h"


namespace se
{
	namespace net
	{
		std::string toString(const boost::asio::ip::udp::endpoint& endpoint)
		{
			return endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
		}

		Endpoint commandLineArgumentsToEndpoint(const int argc, const char** argv)
		{
			if (argc < 3)
			{
				log::info("Provided command line arguments cannot be used to form an endpoint: too few arguments.");
				return Endpoint::invalid;
			}

			const net::Endpoint endpoint(std::string(argv[1]), Port::ValueType(std::atoi(argv[2])));

			//Some endpoint validation...
			int periodCount = 0;
			bool invalidCharacters = false;
			for (size_t i = 0; i < endpoint.address.value.size(); i++)
			{
				if (endpoint.address.value[i] == '.')
					periodCount++;
				else if (endpoint.address.value[i] < 48 || endpoint.address.value[i] > 57)
					invalidCharacters = true;
			}
			if (periodCount != 3 || invalidCharacters || endpoint.address.value.size() > 15)
			{
				log::warning("Provided server address is invalid: " + endpoint.address.toString());
				return Endpoint::invalid;
			}
			if (endpoint.port.value < 0 || endpoint.port.value > std::numeric_limits<uint16_t>::max())
			{
				log::warning("Provided server port is invalid: " + std::string(argv[2]));
				return Endpoint::invalid;
			}

			return endpoint;
		}
	}
}