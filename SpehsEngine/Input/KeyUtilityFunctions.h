#pragma once
#include "SpehsEngine/Input/Key.h"

namespace se
{
	namespace input
	{
		bool isKeyDown(const Key key);
		bool isCtrlDown();
		bool isShiftDown();
	}
}