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
			
			const std::vector<KeyboardEvent>& getKeyboardEvents() const { return keyboardEvents; }
			const std::vector<TextInputEvent>& getTextInputEvents() const { return textInputEvents; }
			const std::vector<MouseHoverEvent>& getMouseHoverEvents() const { return mouseHoverEvents; }
			const std::vector<MouseButtonEvent>& getMouseButtonEvents() const { return mouseButtonEvents; }
			const std::vector<MouseMotionEvent>& getMouseMotionEvents() const { return mouseMotionEvents; }
			const std::vector<MouseWheelEvent>& getMouseWheelEvents() const { return mouseWheelEvents; }
			//const std::vector<JoystickButtonEvent>& getJoystickButtonPressEvents() const { return joystickButtonEvents; }
			//const std::vector<JoystickAxisEvent>& getJoystickAxisEvents() const { return joystickAxisEvents; }
			const std::vector<QuitEvent>& getQuitEvents() const { return quitEvents; }
			const std::vector<FileDropEvent>& getFileDropEvents() const { return fileDropEvents; }

		private:

			std::vector<KeyboardEvent> keyboardEvents;
			std::vector<TextInputEvent> textInputEvents;
			std::vector<MouseHoverEvent> mouseHoverEvents;
			std::vector<MouseButtonEvent> mouseButtonEvents;
			std::vector<MouseMotionEvent> mouseMotionEvents;
			std::vector<MouseWheelEvent> mouseWheelEvents;
			//std::vector<JoystickButtonEvent> joystickButtonEvents;
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