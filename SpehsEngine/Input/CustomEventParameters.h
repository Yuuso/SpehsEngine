#pragma once

#include "SpehsEngine/Input/JoystickGuid.h"


namespace se
{
	namespace input
	{
		// Custom events autogenerate some values based on the original event type
		struct ICustomEvent
		{
			enum class Type : uint8_t
			{
				None = 0,
				Press = 1 << 0,
				Hold = 1 << 1,
				Release = 1 << 2,
				Int = 1 << 3,
				Float = 1 << 4,
				Vec2 = 1 << 5,
				Ivec2 = 1 << 6,
			};

			struct Value
			{
				int i = 0;
				float f = 0.0f;
				glm::vec2 vec2;
				glm::ivec2 ivec2;
			};

			virtual ~ICustomEvent() {}

			bool isHold() const		{ return checkBit(type, Type::Hold); }
			bool isPress() const	{ return checkBit(type, Type::Press); }
			bool isRelease() const	{ return checkBit(type, Type::Release); }

			Value value;
			Type type = Type::None;
		};

		/*
			Each event type needs its set of parameters.
			The check() function is used to determine if the underlying event type should trigger the custom event.
			The set() function can be used for filling up custom event metadata that will be visible on the receiving end.
		*/
		struct KeyboardEventParameters
		{
			bool check(const KeyboardEvent& event) const;
			void set(ICustomEvent& customEvent, const KeyboardEvent& event) const;
			bool operator==(const KeyboardEventParameters& other) const;

			Key key = Key(0);
		};

		struct MouseButtonEventParameters
		{
			bool check(const MouseButtonEvent& event) const;
			void set(ICustomEvent& customEvent, const MouseButtonEvent& event) const;
			bool operator==(const MouseButtonEventParameters& other) const;

			MouseButton mouseButton = MouseButton::none;
		};

		struct MouseMotionEventParameters
		{
			bool check(const MouseMotionEvent&) const;
			void set(ICustomEvent& customEvent, const MouseMotionEvent& event) const;
			bool operator==(const MouseMotionEventParameters&) const;
		};

		struct MouseWheelEventParameters
		{
			bool check(const MouseWheelEvent&) const;
			void set(ICustomEvent& customEvent, const MouseWheelEvent& event) const;
			bool operator==(const MouseWheelEventParameters&) const;
		};

		struct MouseHoverEventParameters
		{
			bool check(const MouseHoverEvent&) const;
			void set(ICustomEvent& customEvent, const MouseHoverEvent& event) const;
			bool operator==(const MouseHoverEventParameters&) const;
		};

		struct JoystickButtonEventParameters
		{
			bool check(const JoystickButtonEvent& event) const;
			void set(ICustomEvent& customEvent, const JoystickButtonEvent& event) const;
			bool operator==(const JoystickButtonEventParameters& other) const;

			JoystickGuid joystickGuid;
			uint8_t buttonIndex = 0;
		};

		struct JoystickAxisEventParameters
		{
			bool check(const JoystickAxisEvent& event) const;
			void set(ICustomEvent& customEvent, const JoystickAxisEvent& event) const;
			bool operator==(const JoystickAxisEventParameters& other) const;

			JoystickGuid joystickGuid;
			uint8_t axisIndex = 0;
		};

		struct JoystickHatEventParameters
		{
			bool check(const JoystickHatEvent& event) const;
			void set(ICustomEvent& customEvent, const JoystickHatEvent& event) const;
			bool operator==(const JoystickHatEventParameters& other) const;

			JoystickGuid joystickGuid;
			uint8_t hatIndex = 0;
		};

		struct CustomEventParameters
		{
			bool operator==(const CustomEventParameters& other) const;

			std::string toString() const;
			// Returns a shorter string, better suited for texts in the UI
			std::string toShortString() const;

			EventType eventType = EventType(0);
			KeyboardEventParameters keyboardEventParameters;
			MouseButtonEventParameters mouseButtonEventParameters;
			MouseMotionEventParameters mouseMotionEventParameters;
			MouseWheelEventParameters mouseWheelEventParameters;
			MouseHoverEventParameters mouseHoverEventParameters;
			JoystickButtonEventParameters joystickButtonEventParameters;
			JoystickAxisEventParameters joystickAxisEventParameters;
			JoystickHatEventParameters joystickHatEventParameters;
		};
	}
}