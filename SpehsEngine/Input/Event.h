#pragma once

#include "SpehsEngine/Input/InputEnumerations.h"
#include "SpehsEngine/Input/Key.h"
#include "SpehsEngine/Input/MouseButton.h"
#include "SpehsEngine/Input/JoystickId.h"
#include "SpehsEngine/Input/JoystickGuid.h"
#include "SpehsEngine/Input/JoystickHatState.h"
#include "glm/vec2.hpp"
#include <string>


namespace se
{
	namespace input
	{
		enum class EventType
		{
			none,
			keyboard,
			textInput,
			mouseButton,
			mouseMotion,
			mouseWheel,
			mouseHover,
			joystickButton,
			joystickAxis,
			joystickHat,
			quit,
			fileDrop,
		};

		std::string toString(const EventType eventType);

		struct KeyboardEvent
		{
			enum class Type : uint8_t { None, Press, Hold, Release };
			static EventType getEventTypeStatic() { return EventType::keyboard; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const KeyboardEvent& other) const { return key == other.key && scancode == other.scancode && type == other.type; }
			bool isPress() const { return type == Type::Press; }
			bool isHold() const { return type == Type::Hold; }
			bool isRelease() const { return type == Type::Release; }
			Key key = Key::UNKNOWN;
			int scancode = 0;
			Type type = Type::None;
		};

		struct TextInputEvent
		{
			static EventType getEventTypeStatic() { return EventType::textInput; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const TextInputEvent& other) const { return buffer == other.buffer; }
			std::string buffer;
		};

		struct MouseButtonEvent
		{
			enum class Type : uint8_t { None, Press, Hold, Release };
			static EventType getEventTypeStatic() { return EventType::mouseButton; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const MouseButtonEvent& other) const { return button == other.button && type == other.type; }
			bool isPress() const { return type == Type::Press; }
			bool isHold() const { return type == Type::Hold; }
			bool isRelease() const { return type == Type::Release; }
			MouseButton button = MouseButton::none;
			Type type = Type::None;
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

		struct JoystickButtonEvent
		{
			enum class Type : uint8_t { None, Press, Hold, Release };
			static EventType getEventTypeStatic() { return EventType::joystickButton; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickButtonEvent& other) const { return joystickId == other.joystickId && buttonIndex == other.buttonIndex && type == other.type && joystickGuid == other.joystickGuid; }
			bool isPress() const { return type == Type::Press; }
			bool isHold() const { return type == Type::Hold; }
			bool isRelease() const { return type == Type::Release; }

			JoystickGuid joystickGuid;
			JoystickId joystickId;
			uint8_t buttonIndex = 0;
			Type type = Type::None;
		};
		struct JoystickAxisEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickAxis; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickAxisEvent& other) const { return joystickId == other.joystickId && axisIndex == other.axisIndex && axisState == other.axisState && joystickGuid == other.joystickGuid; }
			JoystickGuid joystickGuid;
			JoystickId joystickId;
			int32_t axisState = 0;
			uint8_t axisIndex = 0;
		};
		struct JoystickHatEvent
		{
			static EventType getEventTypeStatic() { return EventType::joystickHat; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const JoystickHatEvent& other) const { return joystickId == other.joystickId && hatIndex == other.hatIndex && joystickHatState == other.joystickHatState && joystickGuid == other.joystickGuid; }
			JoystickGuid joystickGuid;
			JoystickId joystickId;
			uint8_t hatIndex = 0;
			JoystickHatState joystickHatState = JoystickHatState::Center;
		};

		struct QuitEvent
		{
			static EventType getEventTypeStatic() { return EventType::quit; }
			EventType getEventType() const { return getEventTypeStatic(); }
			bool operator==(const QuitEvent& /*other*/) const { return true; }
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
