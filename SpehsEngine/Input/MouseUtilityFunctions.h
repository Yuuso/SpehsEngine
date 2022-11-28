#pragma once

#include "SpehsEngine/Input/MouseButton.h"
#include "glm/vec2.hpp"
#include <optional>


namespace se
{
	namespace input
	{
		/* Mouse position is relative to the currently focused window. */
		std::optional<glm::ivec2> getMousePosition();
		std::optional<glm::vec2> getMousePositionf();
		void setMousePosition(const glm::ivec2& _pos);

		bool isMouseButtonDown(const MouseButton mouseButton);

		/* NOTE: this feature has prevented certain SDL input events from being generated (such as the middle mouse button release). */
		bool setRelativeMouseMode(const bool enabled);
		bool getRelativeMouseMode();

		bool setShowCursor(const bool enabled);
		bool getShowCursor();
	}
}
