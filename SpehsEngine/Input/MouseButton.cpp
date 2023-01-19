#include "stdafx.h"
#include "SpehsEngine/Input/MouseButton.h"

namespace se
{
	namespace input
	{
		const char* toString(const MouseButton mouseButton)
		{
			switch (mouseButton)
			{
			case MouseButton::None: return "none";
			case MouseButton::Left: return "left";
			case MouseButton::Middle: return "middle";
			case MouseButton::Right: return "right";
			case MouseButton::X1: return "x1";
			case MouseButton::X2: return "x2";
			}
			return "Error";
		}

		const char* toShortString(const MouseButton mouseButton)
		{
			switch (mouseButton)
			{
			case MouseButton::None: return "none";
			case MouseButton::Left: return "LMB";
			case MouseButton::Middle: return "MMB";
			case MouseButton::Right: return "RMB";
			case MouseButton::X1: return "MB1";
			case MouseButton::X2: return "MB2";
			}
			return "Error";
		}
	}
}
