#pragma once

#include "SpehsEngine/Net/Address.h"
#include "SpehsEngine/Net/Port.h"


namespace se
{
	namespace net
	{
		/*
			Currently spehs uses the standard IPv4 endpoints that consist of a 32 bit address (presented in a string form) and a 16 bit port number.
		*/
		struct Endpoint
		{
			struct HashFunctor
			{
				size_t operator()(const Endpoint& endpoint) const;
			};
			static const Endpoint invalid;
			Endpoint() : address(invalid.address), port(invalid.port) {}
			Endpoint(const Address& _address, const Port& _port) : address(_address), port(_port) {}
			bool operator==(const Endpoint& other) const { return port == other.port && address == other.address; }
			bool operator!=(const Endpoint& other) const { return port != other.port || address != other.address; }
			explicit operator bool() const { return (bool)port && (bool)address; }
			std::string toString() const { return address.toString() + ":" + port.toString(); }
			Address address;
			Port port;
		};
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& writeBuffer, const net::Endpoint& endpoint);
	bool readFromBuffer(ReadBuffer& readBuffer, net::Endpoint& endpoint);
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::Endpoint>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.address, "address");
	se_serial(_serial, _value.port, "port");
	return true;
}

namespace std
{
	template<> struct hash<se::net::Endpoint>
	{
		size_t operator()(const se::net::Endpoint& endpoint) const
		{
			return se::net::Endpoint::HashFunctor()(endpoint);
		}
	};
}
