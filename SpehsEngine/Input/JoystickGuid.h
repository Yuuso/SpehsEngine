#pragma once

#include "SpehsEngine/Core/ByteView.h"
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
	if constexpr (S::getWritingEnabled())
	{
		const ConstStaticByteView<16> byteView((const std::byte*)&_value.data[0]);
		se_serial(_serial, byteView, "JoystickGuid");
		return true;
	}
	else
	{
		StaticByteView<16> byteVector((std::byte*)&_value.data[0]);
		se_serial(_serial, byteVector, "JoystickGuid");
		return true;
	}
}
