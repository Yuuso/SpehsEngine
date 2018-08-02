#pragma once
#include <cstdint>
#include "SpehsEngine/Core/WriteBuffer.h"
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/Assert.h"

namespace se
{
	template<typename UnsignedType>
	void compress(const float value, WriteBuffer& writeBuffer, const float min, const float max)
	{
		static_assert(std::is_integral<UnsignedType>::value, "UnsignedType must be integral type.");
		static_assert(std::is_unsigned<UnsignedType>::value, "UnsignedType must be unsigned type.");
		se_assert(value >= min && value <= max);
		const UnsignedType compressed = std::round(value * std::numeric_limits<UnsignedType>::max());
		writeBuffer.write(compressed);
	}

	template<typename UnsignedType>
	void uncompress(float& value, ReadBuffer& readBuffer, const float min, const float max)
	{
		static_assert(std::is_integral<UnsignedType>::value, "UnsignedType must be integral type.");
		static_assert(std::is_unsigned<UnsignedType>::value, "UnsignedType must be unsigned type.");
		UnsignedType compressed;
		readBuffer.read(compressed);
		value = float(compressed) / float(std::numeric_limits<UnsignedType>::max());
		se_assert(value >= min && value <= max);
	}
}