#pragma once
#include "SpehsEngine/Input/Event.h"
#include <vector>
#include <unordered_set>

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

			EventCatcher();

			/* Discards all previous events and polls for new ones. */
			void pollEvents();
			
			const std::vector<KeyboardPressEvent>& getKeyboardPressEvents() const { return keyboardPressEvents; }
			const std::vector<KeyboardDownEvent>& getKeyboardDownEvents() const { return keyboardDownEvents; }
			const std::vector<KeyboardReleaseEvent>& getKeyboardReleaseEvents() const { return keyboardReleaseEvents; }
			const std::vector<TextInputEvent>& getTextInputEvents() const { return textInputEvents; }
			const std::vector<MouseHoverEvent>& getMouseHoverEvents() const { return mouseHoverEvents; }
			const std::vector<MouseButtonPressEvent>& getMouseButtonPressEvents() const { return mouseButtonPressEvents; }
			const std::vector<MouseButtonDownEvent>& getMouseButtonDownEvents() const { return mouseButtonDownEvents; }
			const std::vector<MouseButtonReleaseEvent>& getMouseButtonReleaseEvents() const { return mouseButtonReleaseEvents; }
			const std::vector<MouseMotionEvent>& getMouseMotionEvents() const { return mouseMotionEvents; }
			const std::vector<MouseWheelEvent>& getMouseWheelEvents() const { return mouseWheelEvents; }
			//const std::vector<JoystickButtonPressEvent>& getJoystickButtonPressEvents() const { return joystickButtonPressEvents; }
			//const std::vector<JoystickButtonDownEvent>& getJoystickButtonDownEvents() const { return joystickButtonDownEvents; }
			//const std::vector<JoystickButtonReleaseEvent>& getJoystickButtonReleaseEvents() const { return joystickButtonReleaseEvents; }
			//const std::vector<JoystickAxisEvent>& getJoystickAxisEvents() const { return joystickAxisEvents; }
			const std::vector<QuitEvent>& getQuitEvents() const { return quitEvents; }
			const std::vector<FileDropEvent>& getFileDropEvents() const { return fileDropEvents; }

		private:

			std::vector<KeyboardPressEvent> keyboardPressEvents;
			std::vector<KeyboardDownEvent> keyboardDownEvents;
			std::vector<KeyboardReleaseEvent> keyboardReleaseEvents;
			std::vector<TextInputEvent> textInputEvents;
			std::vector<MouseHoverEvent> mouseHoverEvents;
			std::vector<MouseButtonPressEvent> mouseButtonPressEvents;
			std::vector<MouseButtonDownEvent> mouseButtonDownEvents;
			std::vector<MouseButtonReleaseEvent> mouseButtonReleaseEvents;
			std::vector<MouseMotionEvent> mouseMotionEvents;
			std::vector<MouseWheelEvent> mouseWheelEvents;
			//std::vector<JoystickButtonPressEvent> joystickButtonPressEvents;
			//std::vector<JoystickButtonDownEvent> joystickButtonDownEvents;
			//std::vector<JoystickButtonReleaseEvent> joystickButtonReleaseEvents;
			//std::vector<JoystickAxisEvent> joystickAxisEvents;
			std::vector<QuitEvent> quitEvents;
			std::vector<FileDropEvent> fileDropEvents;

			std::unordered_set<Key> heldKeyboardKeys;
			std::unordered_set<MouseButton> heldMouseButtons;
			bool previousMousePositionSet = false;
			glm::vec2 previousMousePosition;
		};
	}
}