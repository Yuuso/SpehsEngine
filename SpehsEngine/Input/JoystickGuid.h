#pragma once

#include <stdint.h>


namespace se
{
	namespace input
	{
		struct JoystickGuid
		{
			inline uint8_t operator[](const size_t index) const { return data[index]; }
			inline bool operator==(const JoystickGuid& other) const { return memcmp(data, other.data, 16) == 0; }

			uint8_t data[16] = { 0 };
		};
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::JoystickGuid>::serial(S& _serial, T _value)
{
	constexpr size_t dataSize = 16;
	static_assert(sizeof(_value.data) == dataSize);
	if constexpr (S::getWritingEnabled())
	{
		const ByteView byteView((const std::byte*)&_value.data[0], dataSize);
		se_serial(_serial, byteView, "JoystickGuid");
		return true;
	}
	else
	{
		ByteVector byteVector;
		se_serial(_serial, byteVector, "JoystickGuid");
		if (byteVector.getSize() == dataSize)
		{
			memcpy(&_value.data[0], (const void*)byteVector.getData(), dataSize);
		}
		return true;
	}
}
