#pragma once

#include "SpehsEngine/Input/Event.h"


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
			const std::vector<JoystickButtonEvent>& getJoystickButtonEvents() const { return joystickButtonEvents; }
			const std::vector<JoystickAxisEvent>& getJoystickAxisEvents() const { return joystickAxisEvents; }
			const std::vector<JoystickHatEvent>& getJoystickHatEvents() const { return joystickHatEvents; }
			const std::vector<QuitEvent>& getQuitEvents() const { return quitEvents; }
			const std::vector<FileDropEvent>& getFileDropEvents() const { return fileDropEvents; }

		private:

			struct JoystickState
			{
				std::unordered_set<uint8_t> heldButtons;
				std::optional<JoystickGuid> joystickGuid; // Store guid in case the joystick disconnects
			};

			std::vector<KeyboardEvent> keyboardEvents;
			std::vector<TextInputEvent> textInputEvents;
			std::vector<MouseHoverEvent> mouseHoverEvents;
			std::vector<MouseButtonEvent> mouseButtonEvents;
			std::vector<MouseMotionEvent> mouseMotionEvents;
			std::vector<MouseWheelEvent> mouseWheelEvents;
			std::vector<JoystickButtonEvent> joystickButtonEvents;
			std::vector<JoystickAxisEvent> joystickAxisEvents;
			std::vector<JoystickHatEvent> joystickHatEvents;
			std::vector<QuitEvent> quitEvents;
			std::vector<FileDropEvent> fileDropEvents;

			std::unordered_set<Key> heldKeyboardKeys;
			std::unordered_set<MouseButton> heldMouseButtons;
			std::unordered_map<JoystickId, JoystickState> joystickStates;
			bool previousMousePositionSet = false;
			glm::vec2 previousMousePosition;
		};
	}
}