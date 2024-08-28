#pragma once

#include "SpehsEngine/Input/JoystickId.h"
#include "SpehsEngine/Input/JoystickGuid.h"


namespace se
{
	namespace input
	{
		enum class EventType : uint8_t
		{
			none			= 0,
			keyboard		= 1,
			textInput		= 2,
			mouseButton		= 3,
			mouseMotion		= 4,
			mouseWheel		= 5,
			mouseHover		= 6,
			joystickButton	= 7,
			joystickAxis	= 8,
			joystickHat		= 9,
			quit			= 10,
			fileDrop		= 11,
		};

		std::string toString(const EventType eventType);

		struct KeyboardEvent
		{
			enum class Type : uint8_t { None, Press, Hold, Release };
			static constexpr EventType getEventTypeStatic() { return EventType::keyboard; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const KeyboardEvent& other) const { return key == other.key && scancode == other.scancode && type == other.type; }
			constexpr bool isPress() const { return type == Type::Press; }
			constexpr bool isHold() const { return type == Type::Hold; }
			constexpr bool isRelease() const { return type == Type::Release; }
			Key key = Key(0);
			int scancode = 0;
			Type type = Type::None;
		};

		struct TextInputEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::textInput; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const TextInputEvent& other) const { return buffer == other.buffer; }
			std::string buffer;
		};

		struct MouseButtonEvent
		{
			enum class Type : uint8_t { None, Press, Hold, Release };
			static constexpr EventType getEventTypeStatic() { return EventType::mouseButton; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const MouseButtonEvent& other) const { return button == other.button && type == other.type; }
			constexpr bool isPress() const { return type == Type::Press; }
			constexpr bool isHold() const { return type == Type::Hold; }
			constexpr bool isRelease() const { return type == Type::Release; }
			MouseButton button = MouseButton(0);
			Type type = Type::None;
		};
		struct MouseMotionEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::mouseMotion; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const MouseMotionEvent& other) const { return position == other.position && previousPosition == other.previousPosition && translation == other.translation; }
			glm::vec2 position;
			glm::vec2 previousPosition;
			glm::vec2 translation;
		};
		struct MouseWheelEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::mouseWheel; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const MouseWheelEvent& other) const { return delta == other.delta; }
			glm::ivec2 delta;
		};
		struct MouseHoverEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::mouseHover; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const MouseHoverEvent& other) const { return position == other.position; }
			glm::vec2 position;
		};

		struct JoystickButtonEvent
		{
			enum class Type : uint8_t { None, Press, Hold, Release };
			static constexpr EventType getEventTypeStatic() { return EventType::joystickButton; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const JoystickButtonEvent& other) const { return joystickId == other.joystickId && buttonIndex == other.buttonIndex && type == other.type && joystickGuid == other.joystickGuid; }
			constexpr bool isPress() const { return type == Type::Press; }
			constexpr bool isHold() const { return type == Type::Hold; }
			constexpr bool isRelease() const { return type == Type::Release; }
			JoystickGuid joystickGuid;
			JoystickId joystickId;
			uint8_t buttonIndex = 0;
			Type type = Type::None;
		};
		struct JoystickAxisEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::joystickAxis; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const JoystickAxisEvent& other) const { return joystickId == other.joystickId && axisIndex == other.axisIndex && axisState == other.axisState && joystickGuid == other.joystickGuid; }
			JoystickGuid joystickGuid;
			JoystickId joystickId;
			int32_t axisState = 0;
			uint8_t axisIndex = 0;
		};
		struct JoystickHatEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::joystickHat; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const JoystickHatEvent& other) const { return joystickId == other.joystickId && hatIndex == other.hatIndex && joystickHatState == other.joystickHatState && joystickGuid == other.joystickGuid; }
			JoystickGuid joystickGuid;
			JoystickId joystickId;
			uint8_t hatIndex = 0;
			JoystickHatState joystickHatState = JoystickHatState(0);
		};

		struct QuitEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::quit; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const QuitEvent& /*other*/) const { return true; }
		};

		struct FileDropEvent
		{
			static constexpr EventType getEventTypeStatic() { return EventType::fileDrop; }
			constexpr EventType getEventType() const { return getEventTypeStatic(); }
			constexpr bool operator==(const FileDropEvent& other) const { return filepath == other.filepath; }
			std::string filepath;
		};
	}
}
