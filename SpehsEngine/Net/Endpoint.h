#pragma once
#include "SpehsEngine/Net/Address.h"
#include "SpehsEngine/Net/Port.h"

namespace spehs
{
	namespace net
	{
		/*
			Currently spehs uses the standard IPv4 endpoints that consist of a 32 bit address (presented in a string form) and a 16 bit port number.
		*/
		struct Endpoint
		{
			static const Endpoint invalid;
			Endpoint() : address(invalid.address), port(invalid.port) {}
			Endpoint(const Address& _address, const Port& _port) : address(_address), port(_port) {}
			bool operator==(const Endpoint& other) const { return port == other.port && address == other.address; }
			std::string toString() const { return address.toString() + ":" + port.toString(); }
			Address address;
			Port port;
		};
		Endpoint commandLineArgumentsToEndpoint(const int argc, const char** argv);
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& buffer, const net::Endpoint& endpoint);
	void readFromBuffer(ReadBuffer& buffer, net::Endpoint& endpoint);
}