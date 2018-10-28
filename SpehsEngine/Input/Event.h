#pragma once
#include "SpehsEngine/Input/InputEnumerations.h"
#include "glm/vec2.hpp"
#include <string>

namespace se
{
	namespace input
	{
		enum class EventType
		{
			none,
			keyboardPress,
			keyboardRelease,
			mouseButtonPress,
			mouseButtonRelease,
			mouseMotion,
			mouseWheel,
			mouseHover,
			joystickButtonPress,
			joystickButtonRelease,
			joystickAxis,
			quit,
			fileDrop,
		};

		struct KeyboardPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			KeyboardPressEvent() = default;
			KeyboardPressEvent(const KeyboardKey _key)
				: key(_key) {}
			bool used = false;
			KeyboardKey key = KeyboardKey::KEYBOARD_UNKNOWN;
		};
		struct KeyboardReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			KeyboardReleaseEvent() = default;
			KeyboardReleaseEvent(const KeyboardKey _key)
				: key(_key) {}
			bool used = false;
			KeyboardKey key = KeyboardKey::KEYBOARD_UNKNOWN;
		};

		struct MouseButtonPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseButtonPressEvent() = default;
			MouseButtonPressEvent(const KeyboardKey _button)
				: button(_button) {}
			bool used = false;
			KeyboardKey button = KeyboardKey::KEYBOARD_UNKNOWN;
		};
		struct MouseButtonReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseButtonReleaseEvent() = default;
			MouseButtonReleaseEvent(const KeyboardKey _button)
				: button(_button) {}
			bool used = false;
			KeyboardKey button = KeyboardKey::KEYBOARD_UNKNOWN;
		};
		struct MouseMotionEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseMotion; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseMotionEvent() = default;
			MouseMotionEvent(const glm::vec2& _position, const glm::vec2& _previousPosition)
				: position(_position), previousPosition(_previousPosition) {}
			bool used = false;
			glm::vec2 position;
			glm::vec2 previousPosition;
		};
		struct MouseWheelEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseWheel; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseWheelEvent() = default;
			MouseWheelEvent(const int _wheelDelta)
				: wheelDelta(_wheelDelta) {}
			bool used = false;
			int wheelDelta = 0;
		};
		struct MouseHoverEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseHover; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseHoverEvent() = default;
			bool used = false;
		};

		struct JoystickButtonPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			JoystickButtonPressEvent(const KeyboardKey _button, const int _axisIndex, const float _axisMovement, const Joystick* const _joystick)
				: joystick(_joystick), button(_button) {}
			bool used = false;
			const Joystick* const joystick;
			const KeyboardKey button;
		};
		struct JoystickButtonReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			JoystickButtonReleaseEvent(const Joystick* const _joystick, const KeyboardKey _button)
				: joystick(_joystick), button(_button) {}
			bool used = false;
			const Joystick* const joystick;
			const KeyboardKey button;
		};
		struct JoystickAxisEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickAxis; }
			EventType getEventType() const { return getEventTypeStatic(); }
			JoystickAxisEvent(const Joystick* const _joystick, const int _axisIndex, const float _axisMovement)
				: joystick(_joystick), axisIndex(_axisIndex), axisMovement(_axisMovement) {}
			bool used = false;
			const Joystick* const joystick;
			const int axisIndex;
			const float axisMovement;
		};

		struct QuitEvent
		{
			static EventType getEventTypeStatic() { return EventType::quit; }
			EventType getEventType() const { return getEventTypeStatic(); }
			QuitEvent() = default;
			bool used = false;
		};

		struct FileDropEvent
		{
			static EventType getEventTypeStatic() { return EventType::fileDrop; }
			EventType getEventType() const { return getEventTypeStatic(); }
			FileDropEvent() = default;
			FileDropEvent(const std::string& _filepath)
				: filepath(_filepath) {}
			bool used = false;
			std::string filepath;
		};
	}
}
