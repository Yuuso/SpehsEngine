#pragma once
#include "GUIRectangle.h"

namespace spehs
{
	/**Base class/interface for "editor" GUI classes*/
	class GUIEditor : public spehs::GUIRectangle
	{
		/**Returns true when user has edited the value and the value is ready to be processed. Should return true during a full program loop cycle (from input update to the next)*/
		virtual bool valueEdited() const = 0;
	};
}