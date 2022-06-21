#pragma once
#include <string>

namespace se
{
	namespace net
	{
		struct Port
		{
			static const Port invalid;
			typedef uint16_t ValueType;
			Port();
			Port(const ValueType& _value) : value(_value) {}
			operator ValueType() const { return value; }
			bool operator==(const Port& other) const { return value == other.value; }
			bool operator!=(const Port& other) const { return value != other.value; }
			explicit operator bool() const { return *this != invalid; }
			std::string toString() const { return std::to_string(value); }
			ValueType value;
		};
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& writeBuffer, const net::Port& port);
	bool readFromBuffer(ReadBuffer& readBuffer, net::Port& port);
}
