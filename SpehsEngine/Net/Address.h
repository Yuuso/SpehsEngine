#pragma once
#include <string>

namespace se
{
	namespace net
	{
		struct Address
		{
			static const Address invalid;
			typedef std::string ValueType;
			Address();
			Address(const ValueType& _value) : value(_value) {}
			operator ValueType() const { return value; }
			bool operator==(const Address& other) const { return value == other.value; }
			bool operator!=(const Address& other) const { return value != other.value; }
			operator bool() const { return *this != invalid; }
			std::string toString() const { return value; }
			ValueType value;
		};
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& writeBuffer, const net::Address& address);
	bool readFromBuffer(ReadBuffer& readBuffer, net::Address& address);
}