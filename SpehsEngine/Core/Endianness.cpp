#include "Endianness.h"

namespace spehs
{
	std::string getEndiannessAsString(const Endianness endianness)
	{
		switch (endianness)
		{
		case Endianness::big: return "big";
		case Endianness::little: return "little";
		default: return "unknown";
		}
	}

	enum
	{
		O32_LITTLE_ENDIAN = 0x03020100ul,
		O32_BIG_ENDIAN = 0x00010203ul,
		O32_PDP_ENDIAN = 0x01000302ul
	};
	static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order = { { 0, 1, 2, 3 } };

	const Endianness networkByteOrder = Endianness::little;
	const Endianness hostByteOrder = o32_host_order.value == O32_BIG_ENDIAN ? Endianness::big : (o32_host_order.value == O32_LITTLE_ENDIAN ? Endianness::little : Endianness::unknown);
}