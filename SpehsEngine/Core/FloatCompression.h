#pragma once
#include <cstdint>
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/SE_Assert.h"

namespace se
{
	template<typename UnsignedType>
	void compress(const float value, WriteBuffer& writeBuffer, const float min, const float max)
	{
		static_assert(std::is_integral<UnsignedType>::value, "UnsignedType must be integral type.");
		static_assert(std::is_unsigned<UnsignedType>::value, "UnsignedType must be unsigned type.");
		se_assert(value >= min && value <= max);
		const UnsignedType compressed = UnsignedType(std::round(value * std::numeric_limits<UnsignedType>::max()));
		writeBuffer.write(compressed);
	}

	template<typename UnsignedType>
	bool uncompress(float& value, ReadBuffer& readBuffer, const float min, const float max)
	{
		static_assert(std::is_integral<UnsignedType>::value, "UnsignedType must be integral type.");
		static_assert(std::is_unsigned<UnsignedType>::value, "UnsignedType must be unsigned type.");
		UnsignedType compressed;
		se_read(readBuffer, compressed);
		value = float(compressed) / float(std::numeric_limits<UnsignedType>::max());
		se_assert(value >= min && value <= max);
		return true;
	}
}