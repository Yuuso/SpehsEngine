#pragma once


namespace se
{
	namespace input
	{
		enum class MouseButton : uint32_t
		{
			None = 0,
			Left = 1,
			Middle = 2,
			Right = 3,
			X1 = 4,
			X2 = 5,
		};

		const char* toString(const MouseButton mouseButton);
		const char* toShortString(const MouseButton mouseButton);
	}
}
