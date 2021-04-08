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
			default: se_assert(false && "invalid mouseButton"); return "error";
			case MouseButton::left: return "left";
			case MouseButton::middle: return "middle";
			case MouseButton::right: return "right";
			case MouseButton::x1: return "x1";
			case MouseButton::x2: return "x2";
			}
			return "Error";
		}
	}
}
