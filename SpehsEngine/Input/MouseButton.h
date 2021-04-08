#pragma once
#include <string>

namespace se
{
	namespace input
	{
		enum class MouseButton
		{
			none = 0,
			left = 1,
			middle = 2,
			right = 3,
			x1 = 4,
			x2 = 5,
		};

		const char* toString(const MouseButton mouseButton);
	}
}
