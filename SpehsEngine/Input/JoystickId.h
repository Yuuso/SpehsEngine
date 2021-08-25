#pragma once

#include "SpehsEngine/Core/StrongInt.h"


namespace se
{
	namespace input
	{
		// Joystick id stays constant while the joystick remains connected
		SE_STRONG_INT(int32_t, JoystickId, -1);
	}
}

SE_STRONG_INT_STD(se::input::JoystickId);
