#pragma once

#include <stdint.h>


namespace se
{
	namespace input
	{
		enum class JoystickHatState : uint8_t
		{
			Center = 0x00,
			Up = 0x01,
			Right = 0x02,
			Down = 0x03,
			Left = 0x08,
			RightUp = 0x03,
			RightDown = 0x05,
			LeftUp = 0x09,
			LeftDown = 0x0B
		};
	}
}
