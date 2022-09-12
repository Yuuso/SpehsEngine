#include "stdafx.h"
#include "Endpoint.h"

#include <SpehsEngine/Core/Log.h>
#include <SpehsEngine/Core/ReadBuffer.h>
#include <SpehsEngine/Core/WriteBuffer.h>
#include <SpehsEngine/Core/Murmur3.h>

namespace se
{
	namespace net
	{
		const Endpoint Endpoint::invalid = Endpoint(Address::invalid, Port::invalid);

		size_t Endpoint::HashFunctor::operator()(const Endpoint& endpoint) const
		{
			return murmurHash3_x86_32(endpoint.address.value.data(), endpoint.address.value.size(), 0) ^ uint32_t(endpoint.port.value);
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