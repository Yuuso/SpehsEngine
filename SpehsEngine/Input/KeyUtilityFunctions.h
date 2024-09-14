#pragma once


namespace se
{
	namespace input
	{
		bool isKeyDown(const Key key);
		bool isCtrlDown();
		bool isShiftDown();
		bool isAltDown(); // or AltGr
		int getScancode(const Key key);
	}
}
