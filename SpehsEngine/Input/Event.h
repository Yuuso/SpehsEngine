#pragma once
#include "SpehsEngine/Input/InputEnumerations.h"
#include "SpehsEngine/Input/Key.h"
#include "SpehsEngine/Input/MouseButton.h"
#include "glm/vec2.hpp"
#include <string>

namespace se
{
	namespace input
	{
		class Joystick;

		enum class EventType
		{
			none,
			keyboardPress,
			keyboardDown,
			keyboardRelease,
			textInput,
			mouseButtonPress,
			mouseButtonDown,
			mouseButtonRelease,
			mouseMotion,
			mouseWheel,
			mouseHover,
			joystickButtonPress,
			joystickButtonDown,
			joystickButtonRelease,
			joystickAxis,
			quit,
			fileDrop,
		};

		std::string toString(const EventType eventType);

		struct KeyboardPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const KeyboardPressEvent& other) const { return key == other.key && scancode == other.scancode; }
			Key key = Key::UNKNOWN;
			int scancode = 0;
		};
		struct KeyboardDownEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardDown; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const KeyboardDownEvent& other) const { return key == other.key; }
			Key key = Key::UNKNOWN;
		};
		struct KeyboardReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const KeyboardReleaseEvent& other) const { return key == other.key && scancode == other.scancode; }
			Key key = Key::UNKNOWN;
			int scancode = 0;
		};

		struct TextInputEvent
		{
			static EventType getEventTypeStatic() { return EventType::textInput; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const TextInputEvent& other) const { return buffer == other.buffer; }
			std::string buffer;
		};

		struct MouseButtonPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseButtonPressEvent& other) const { return button == other.button; }
			MouseButton button = MouseButton::none;
		};
		struct MouseButtonDownEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonDown; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseButtonDownEvent& other) const { return button == other.button; }
			MouseButton button = MouseButton::none;
		};
		struct MouseButtonReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseButtonReleaseEvent& other) const { return button == other.button; }
			MouseButton button = MouseButton::none;
		};
		struct MouseMotionEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseMotion; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseMotionEvent& other) const { return position == other.position && previousPosition == other.previousPosition && translation == other.translation; }
			glm::vec2 position;
			glm::vec2 previousPosition;
			glm::vec2 translation;
		};
		struct MouseWheelEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseWheel; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseWheelEvent& other) const { return delta == other.delta; }
			glm::ivec2 delta;
		};
		struct MouseHoverEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseHover; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseHoverEvent& other) const { return position == other.position; }
			glm::vec2 position;
		};

		struct JoystickButtonPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickButtonPressEvent& other) const { return joystick == other.joystick && button == other.button; }
			const Joystick* joystick = nullptr;
			KeyboardKey button = KeyboardKey::KEYBOARD_UNKNOWN;
		};
		struct JoystickButtonDownEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonDown; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickButtonDownEvent& other) const { return joystick == other.joystick && button == other.button; }
			const Joystick* joystick = nullptr;
			KeyboardKey button = KeyboardKey::KEYBOARD_UNKNOWN;
		};
		struct JoystickButtonReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickButtonReleaseEvent& other) const { return joystick == other.joystick && button == other.button; }
			const Joystick* joystick = nullptr;
			KeyboardKey button = KeyboardKey::KEYBOARD_UNKNOWN;
		};
		struct JoystickAxisEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickAxis; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickAxisEvent& other) const { return joystick == other.joystick && axisIndex == other.axisIndex && axisMovement == other.axisMovement; }
			const Joystick* joystick = nullptr;
			int axisIndex = 0;
			float axisMovement = 0;
		};

		struct QuitEvent
		{
			static EventType getEventTypeStatic() { return EventType::quit; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const QuitEvent& other) const { return true; }
		};

		struct FileDropEvent
		{
			static EventType getEventTypeStatic() { return EventType::fileDrop; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const FileDropEvent& other) const { return filepath == other.filepath; }
			std::string filepath;
		};
	}
}
