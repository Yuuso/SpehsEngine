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

	template<typename T>
	T swapEndianness(T _value)
	{
		union
		{
			T value;
			uint8_t bytes[sizeof(T)];
		} source, dest;
		source.value = _value;

		for (size_t i = 0; i < sizeof(T); i++)
		{
			dest.bytes[i] = source.bytes[sizeof(T) - i - 1];
		}
		return dest.value;
	}
	template<typename T>
	T networkToHost(T value)
	{
		if (hostByteOrder == networkByteOrder)
		{
			return value;
		}
		return swapEndianness(value);
	}
	template<typename T>
	T hostToNetwork(T value)
	{
		if (hostByteOrder == networkByteOrder)
		{
			return value;
		}
		return swapEndianness(value);
	}
}