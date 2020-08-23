#pragma once
#include "SpehsEngine/Input/MouseButton.h"
#include "glm/vec2.hpp"

namespace se
{
	namespace input
	{
		/* Mouse position is relative to the currently focused window. */
		glm::ivec2 getMousePosition();
		glm::vec2 getMousePositionf();

		bool isMouseButtonDown(const MouseButton mouseButton);

		/* NOTE: this feature has prevented certain SDL input events from being generated (such as the middle mouse button release). */
		bool setRelativeMouseMode(const bool enabled);
		bool getRelativeMouseMode();

		bool setShowCursor(const bool enabled);
		bool getShowCursor();
	}
}
