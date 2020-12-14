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
			Key key = Key::UNKNOWN;
			int scancode = 0;
		};
		struct KeyboardDownEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardDown; }
			EventType getEventType() const { return getEventTypeStatic(); }
			Key key = Key::UNKNOWN;
		};
		struct KeyboardReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::keyboardRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			Key key = Key::UNKNOWN;
			int scancode = 0;
		};

		struct TextInputEvent
		{
			static EventType getEventTypeStatic() { return EventType::textInput; }
			EventType getEventType() const { return getEventTypeStatic(); }
			std::vector<char> buffer;
		};

		struct MouseButtonPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseButton button = MouseButton::none;
		};
		struct MouseButtonDownEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonDown; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseButton button = MouseButton::none;
		};
		struct MouseButtonReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseButtonRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			MouseButton button = MouseButton::none;
		};
		struct MouseMotionEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseMotion; }
			EventType getEventType() const { return getEventTypeStatic(); }
			glm::vec2 position;
			glm::vec2 previousPosition;
			glm::vec2 translation;
		};
		struct MouseWheelEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseWheel; }
			EventType getEventType() const { return getEventTypeStatic(); }
			glm::ivec2 delta;
		};
		struct MouseHoverEvent
		{
			static EventType getEventTypeStatic() { return EventType::mouseHover; }
			EventType getEventType() const { return getEventTypeStatic(); }
			glm::vec2 position;
		};

		struct JoystickButtonPressEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonPress; }
			EventType getEventType() const { return getEventTypeStatic(); }
			const Joystick* joystick = nullptr;
			KeyboardKey button;
		};
		struct JoystickButtonDownEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonDown; }
			EventType getEventType() const { return getEventTypeStatic(); }
			const Joystick* joystick = nullptr;
			KeyboardKey button;
		};
		struct JoystickButtonReleaseEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickButtonRelease; }
			EventType getEventType() const { return getEventTypeStatic(); }
			const Joystick* joystick = nullptr;
			KeyboardKey button;
		};
		struct JoystickAxisEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickAxis; }
			EventType getEventType() const { return getEventTypeStatic(); }
			const Joystick* joystick = nullptr;
			int axisIndex;
			float axisMovement;
		};

		struct QuitEvent
		{
			static EventType getEventTypeStatic() { return EventType::quit; }
			EventType getEventType() const { return getEventTypeStatic(); }
		};

		struct FileDropEvent
		{
			static EventType getEventTypeStatic() { return EventType::fileDrop; }
			EventType getEventType() const { return getEventTypeStatic(); }
			std::string filepath;
		};
	}
}
