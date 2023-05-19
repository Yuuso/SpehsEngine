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
			explicit operator bool() const { return *this != invalid; }
			std::string toString() const { return value; }
			ValueType value;
		};
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::Address>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.value, "value");
	return true;
}
