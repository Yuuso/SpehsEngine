#include "stdafx.h"
#include "SpehsEngine/Net/Endpoint.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/Murmur3.h"

namespace se
{
	namespace net
	{
		const Endpoint Endpoint::invalid = Endpoint(Address::invalid, Port::invalid);

		size_t Endpoint::HashFunctor::operator()(const Endpoint& endpoint) const
		{
			return murmurHash3_x86_32(endpoint.address.value.data(), endpoint.address.value.size(), 0) ^ uint32_t(endpoint.port.value);
		}

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

	void writeToBuffer(WriteBuffer& writeBuffer, const net::Endpoint& endpoint)
	{
		writeBuffer.write(endpoint.address);
		writeBuffer.write(endpoint.port);
	}

	bool readFromBuffer(ReadBuffer& readBuffer, net::Endpoint& endpoint)
	{
		se_read(readBuffer, endpoint.address);
		se_read(readBuffer, endpoint.port);
		return true;
	}
}