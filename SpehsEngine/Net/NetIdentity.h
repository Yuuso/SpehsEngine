#pragma once

#include <stdint.h>
#include <string>
#include <steam/steamnetworkingtypes.h>


namespace se
{
	namespace net
	{
		// Each application run has its own randomly generated identity
		struct NetIdentity
		{
			NetIdentity() = default;
			NetIdentity(const uint64_t _value) : value(_value) {}

			bool operator==(const NetIdentity& other) const { return value == other.value; }
			bool operator!=(const NetIdentity& other) const { return value != other.value; }
			explicit operator bool() const { return value != 0; }
			bool isValid() const { return value != 0; }
			bool fromString(const std::string_view string);
			std::string toString() const;

			uint64_t value = 0;
		};

		NetIdentity getMyNetIdentity();
		SteamNetworkingIdentity toSteamNetworkingIdentity(const NetIdentity& netIdentity);
		NetIdentity fromSteamNetworkingIdentity(const SteamNetworkingIdentity& steamNetworkingIdentity);
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::NetIdentity>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.value, "value");
	return true;
}