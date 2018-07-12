#include "Endpoint.h"

#include <SpehsEngine/Core/Log.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/WriteBuffer.h>

namespace se
{
	namespace net
	{
		const Endpoint Endpoint::invalid = Endpoint(Address::invalid, Port::invalid);

		Endpoint commandLineArgumentsToEndpoint(const int argc, const char** argv)
		{
			if (argc < 3)
			{
				se::log::info("Provided command line arguments cannot be used to form an endpoint: too few arguments.");
				return Endpoint::invalid;
			}

			const net::Endpoint endpoint(std::string(argv[1]), std::atoi(argv[2]));

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
				se::log::warning("Provided server address is invalid: " + endpoint.address.toString());
				return Endpoint::invalid;
			}
			if (endpoint.port.value < 0 || endpoint.port.value > std::numeric_limits<uint16_t>::max())
			{
				se::log::warning("Provided server port is invalid: " + std::string(argv[2]));
				return Endpoint::invalid;
			}

			return endpoint;
		}
	}

	void writeToBuffer(WriteBuffer& buffer, const net::Endpoint& endpoint)
	{
		buffer.write(endpoint.address);
		buffer.write(endpoint.port);
	}

	void readFromBuffer(ReadBuffer& buffer, net::Endpoint& endpoint)
	{
		buffer.read(endpoint.address);
		buffer.read(endpoint.port);
	}
}