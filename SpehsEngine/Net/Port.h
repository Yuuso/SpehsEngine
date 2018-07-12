#pragma once
#include <string>

namespace spehs
{
	namespace net
	{
		struct Port
		{
			static const Port invalid;
			static const Port default;
			typedef uint16_t ValueType;
			Port() = default;
			Port(const ValueType& _value) : value(_value) {}
			operator ValueType() const { return value; }
			bool operator==(const Port& other) const { return value == other.value; }
			bool operator!=(const Port& other) const { return value != other.value; }
			operator bool() const { return *this != invalid; }
			std::string toString() const { return std::to_string(value); }
			ValueType value = 0;
		};
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& buffer, const net::Port& port);
	void readFromBuffer(ReadBuffer& buffer, net::Port& port);
}