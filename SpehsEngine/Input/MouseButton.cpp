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
			case MouseButton::none: return "none";
			case MouseButton::left: return "left";
			case MouseButton::middle: return "middle";
			case MouseButton::right: return "right";
			case MouseButton::x1: return "x1";
			case MouseButton::x2: return "x2";
			}
			return "Error";
		}

		const char* toShortString(const MouseButton mouseButton)
		{
			switch (mouseButton)
			{
			case MouseButton::none: return "none";
			case MouseButton::left: return "LMB";
			case MouseButton::middle: return "MMB";
			case MouseButton::right: return "RMB";
			case MouseButton::x1: return "MB1";
			case MouseButton::x2: return "MB2";
			}
			return "Error";
		}
	}
}
