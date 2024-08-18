#pragma once


namespace se
{
	namespace net
	{
		struct Port
		{
			typedef uint16_t ValueType;
			constexpr Port() = default;
			constexpr Port(const ValueType& _value) : value(_value) {}
			constexpr operator ValueType() const { return value; }
			constexpr bool operator==(const Port& other) const { return value == other.value; }
			constexpr bool operator!=(const Port& other) const { return value != other.value; }
			constexpr explicit operator bool() const { return value != 0u; }
			constexpr bool isValid() const { return value != 0u; }
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
