#include "stdafx.h"
#include "SpehsEngine/Net/Endpoint.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/Murmur3.h"

namespace se
{
	namespace net
	{
		const Endpoint Endpoint::invalid = Endpoint(Address::invalid, Port());

		size_t Endpoint::HashFunctor::operator()(const Endpoint& endpoint) const
		{
			return murmurHash3_x86_32(endpoint.address.value.data(), endpoint.address.value.size(), 0) ^ uint32_t(endpoint.port.value);
		}
	}
}