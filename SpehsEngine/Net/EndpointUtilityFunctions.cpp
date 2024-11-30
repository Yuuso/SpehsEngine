#include "stdafx.h"
#include "SpehsEngine/Net/EndpointUtilityFunctions.h"


namespace se
{
	namespace net
	{
		Endpoint commandLineArgumentsToEndpoint(const int argc, const char** argv)
		{
			if (argc < 3)
			{
				log::info("Provided command line arguments cannot be used to form an endpoint: too few arguments.");
				return Endpoint::invalid;
			}

			const int portValue = std::atoi(argv[2]);
			if (portValue <= 0 || portValue > std::numeric_limits<uint16_t>::max())
			{
				log::warning("Provided server port is invalid: " + std::string(argv[2]));
				return Endpoint::invalid;
			}
			const net::Endpoint endpoint(Address::makeFromStringIpv4(argv[1]), Port::ValueType(portValue));
			if (!endpoint.address.isValid())
			{
				log::warning("Provided server address is invalid: " + endpoint.address.toString());
				return Endpoint::invalid;
			}
			return endpoint;
		}
	}
}