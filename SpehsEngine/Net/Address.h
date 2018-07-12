#pragma once
#include <string>

namespace spehs
{
	namespace net
	{
		struct Address
		{
			static const Address invalid;
			typedef std::string ValueType;
			Address() = default;
			Address(const ValueType& _value) : value(_value) {}
			operator ValueType() const { return value; }
			bool operator==(const Address& other) const { return value == other.value; }
			bool operator!=(const Address& other) const { return value != other.value; }
			operator bool() const { return *this != invalid; }
			std::string toString() const { return value; }
			ValueType value = "0.0.0.0";
		};
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& buffer, const net::Address& address);
	void readFromBuffer(ReadBuffer& buffer, net::Address& address);
}