#pragma once


namespace se
{
	namespace net
	{
		struct Port
		{
			typedef uint16_t ValueType;
			Port() = default;
			Port(const ValueType& _value) : value(_value) {}
			operator ValueType() const { return value; }
			bool operator==(const Port& other) const { return value == other.value; }
			bool operator!=(const Port& other) const { return value != other.value; }
			explicit operator bool() const { return value != 0; }
			std::string toString() const { return std::to_string(value); }
			ValueType value = 0;
		};
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::net::Port>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.value, "value");
	return true;
}
