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
