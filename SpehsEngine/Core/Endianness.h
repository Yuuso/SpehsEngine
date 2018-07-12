#pragma once
#include <string>

namespace se
{
	/* Byte endianness ordering */
	enum class Endianness : uint8_t
	{
		unknown = 0,
		little = 1,
		big = 2,
	};
	extern std::string getEndiannessAsString(const Endianness endianness);
	extern const Endianness hostByteOrder;
	extern const Endianness networkByteOrder;
}