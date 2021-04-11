#pragma once

#include "SpehsEngine/Input/Event.h"


namespace se
{
	namespace input
	{
		struct CustomEventParameters
		{
			bool operator==(const CustomEventParameters& other) const;

			std::string toString() const;
			// Returns a shorter string, better suited for texts in the UI
			std::string toShortString() const;

			EventType eventType = EventType::none;
			KeyboardPressEvent keyboardPressEvent;
			KeyboardDownEvent keyboardDownEvent;
			KeyboardReleaseEvent keyboardReleaseEvent;
			MouseButtonPressEvent mouseButtonPressEvent;
			MouseButtonDownEvent mouseButtonDownEvent;
			MouseButtonReleaseEvent mouseButtonReleaseEvent;
			MouseMotionEvent mouseMotionEvent;
			MouseWheelEvent mouseWheelEvent;
			MouseHoverEvent mouseHoverEvent;
			JoystickButtonPressEvent joystickButtonPressEvent;
			JoystickButtonDownEvent joystickButtonDownEvent;
			JoystickButtonReleaseEvent joystickButtonReleaseEvent;
			JoystickAxisEvent joystickAxisEvent;
			QuitEvent quitEvent;
			FileDropEvent fileDropEvent;
			TextInputEvent textInputEvent;
		};
	}
}