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
			switch (endpoint.address.type)
			{
			case Address::Type::None: break;
			case Address::Type::Ipv4: return size_t(endpoint.address.data0 ^ uint32_t(endpoint.port.value));
			case Address::Type::Ipv6: return size_t(Murmur3::impl((const char*)&endpoint.address.data0, 16, 0) ^ uint32_t(endpoint.port.value));
			}
			return size_t(uint32_t(endpoint.port.value));
		}
	}
}