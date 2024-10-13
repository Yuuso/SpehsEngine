#pragma once

#include "SpehsEngine/Net/NetIdentity.h"


namespace se
{
	namespace net
	{
		struct SignalingEntryPacket
		{
			NetIdentity netIdentity;
		};

		struct SignalingDataPacket
		{
			NetIdentity netIdentity;
			std::vector<uint8_t> data;
		};
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::SignalingEntryPacket>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.netIdentity, "netIdentity");
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::SignalingDataPacket>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.netIdentity, "netIdentity");
	se_serial(_serial, _value.data, "data");
	return true;
}
