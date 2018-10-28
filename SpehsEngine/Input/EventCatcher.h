#pragma once
#include "SpehsEngine/Input/Event.h"
#include <vector>

namespace se
{
	namespace input
	{
		/*
			Used to catch input events. Globally only one input catcher may catch events at a time.
		*/
		class EventCatcher
		{
		public:

			EventCatcher(const rendering::Window& window);

			void update();

			std::vector<KeyboardPressEvent>& getKeyboardPressEvents() { return keyboardPressEvents; }
			std::vector<KeyboardReleaseEvent>& getKeyboardReleaseEvents() { return keyboardReleaseEvents; }
			std::vector<MouseButtonPressEvent>& getMouseButtonPressEvents()  { return mouseButtonPressEvents; }
			std::vector<MouseButtonReleaseEvent>& getMouseButtonReleaseEvents() { return mouseButtonReleaseEvents; }
			std::vector<MouseMotionEvent>& getMouseMotionEvents() { return mouseMotionEvents; }
			std::vector<MouseWheelEvent>& getMouseWheelEvents() { return mouseWheelEvents; }
			std::vector<MouseHoverEvent>& getMouseHoverEvents() { return mouseHoverEvents; }
			//std::vector<JoystickButtonPressEvent>& getJoystickButtonPressEvents() { return joystickButtonPressEvents; }
			//std::vector<JoystickButtonReleaseEvent>& getJoystickButtonReleaseEvents() { return joystickButtonReleaseEvents; }
			//std::vector<JoystickAxisEvent>& getJoystickAxisEvents() { return joystickAxisEvents; }
			std::vector<QuitEvent>& getQuitEvents() { return quitEvents; }
			std::vector<FileDropEvent>& getFileDropEvents() { return fileDropEvents; }

		private:

			const rendering::Window& window; // TO BE REMOVED: currently exists because of flipping mouse position from given sdl coordinates to spehs coordinates.

			std::vector<KeyboardPressEvent> keyboardPressEvents;
			std::vector<KeyboardReleaseEvent> keyboardReleaseEvents;
			std::vector<MouseButtonPressEvent> mouseButtonPressEvents;
			std::vector<MouseButtonReleaseEvent> mouseButtonReleaseEvents;
			std::vector<MouseMotionEvent> mouseMotionEvents;
			std::vector<MouseWheelEvent> mouseWheelEvents;
			std::vector<MouseHoverEvent> mouseHoverEvents;
			//std::vector<JoystickButtonPressEvent> joystickButtonPressEvents;
			//std::vector<JoystickButtonReleaseEvent> joystickButtonReleaseEvents;
			//std::vector<JoystickAxisEvent> joystickAxisEvents;
			std::vector<QuitEvent> quitEvents;
			std::vector<FileDropEvent> fileDropEvents;

			bool previousMousePositionSet = false;
			glm::vec2 previousMousePosition;
		};
	}
}