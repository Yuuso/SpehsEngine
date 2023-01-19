#pragma once

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Input/JoystickUtilityFunctions.h"


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

			bool isPress() const { return type == Type::Press; }
			bool isHold() const { return type == Type::Hold; }
			bool isRelease() const { return type == Type::Release; }

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
			bool check(const KeyboardEvent& event) const
			{
				return event.key == key;
			}
			void set(ICustomEvent& customEvent, const KeyboardEvent& event) const
			{
				switch (event.type)
				{
				case KeyboardEvent::Type::None:
					break;
				case KeyboardEvent::Type::Press:
					customEvent.type = ICustomEvent::Type::Press;
					break;
				case KeyboardEvent::Type::Hold:
					customEvent.type = ICustomEvent::Type::Hold;
					break;
				case KeyboardEvent::Type::Release:
					customEvent.type = ICustomEvent::Type::Release;
					break;
				}
			}
			bool operator==(const KeyboardEventParameters& other) const
			{
				return key == other.key;
			}
			Key key = Key::UNKNOWN;
		};

		struct MouseButtonEventParameters
		{
			bool check(const MouseButtonEvent& event) const
			{
				return event.button == mouseButton;
			}
			void set(ICustomEvent& customEvent, const MouseButtonEvent& event) const
			{
				switch (event.type)
				{
				case MouseButtonEvent::Type::None:
					break;
				case MouseButtonEvent::Type::Press:
					customEvent.type = ICustomEvent::Type::Press;
					break;
				case MouseButtonEvent::Type::Hold:
					customEvent.type = ICustomEvent::Type::Hold;
					break;
				case MouseButtonEvent::Type::Release:
					customEvent.type = ICustomEvent::Type::Release;
					break;
				}
			}
			bool operator==(const MouseButtonEventParameters& other) const
			{
				return mouseButton == other.mouseButton;
			}
			MouseButton mouseButton = MouseButton::None;
		};

		struct MouseMotionEventParameters
		{
			bool check(const MouseMotionEvent&) const
			{
				return true;
			}
			void set(ICustomEvent& customEvent, const MouseMotionEvent& event) const
			{
				customEvent.type = ICustomEvent::Type::Vec2;
				customEvent.value.vec2 = event.translation;
			}
			bool operator==(const MouseMotionEventParameters&) const
			{
				return true;
			}
		};

		struct MouseWheelEventParameters
		{
			bool check(const MouseWheelEvent&) const
			{
				return true;
			}
			void set(ICustomEvent& customEvent, const MouseWheelEvent& event) const
			{
				customEvent.type = ICustomEvent::Type::Ivec2;
				customEvent.value.ivec2 = event.delta;
			}
			bool operator==(const MouseWheelEventParameters&) const
			{
				return true;
			}
		};

		struct MouseHoverEventParameters
		{
			bool check(const MouseHoverEvent&) const
			{
				return true;
			}
			void set(ICustomEvent& customEvent, const MouseHoverEvent& event) const
			{
				customEvent.type = ICustomEvent::Type::Vec2;
				customEvent.value.vec2 = event.position;
			}
			bool operator==(const MouseHoverEventParameters&) const
			{
				return true;
			}
		};

		struct JoystickButtonEventParameters
		{
			bool check(const JoystickButtonEvent& event) const
			{
				return buttonIndex == event.buttonIndex && joystickGuid == getJoystickGuid(event.joystickId);
			}
			void set(ICustomEvent& customEvent, const JoystickButtonEvent& event) const
			{
				switch (event.type)
				{
				case JoystickButtonEvent::Type::None:
					break;
				case JoystickButtonEvent::Type::Press:
					customEvent.type = ICustomEvent::Type::Press;
					break;
				case JoystickButtonEvent::Type::Hold:
					customEvent.type = ICustomEvent::Type::Hold;
					break;
				case JoystickButtonEvent::Type::Release:
					customEvent.type = ICustomEvent::Type::Release;
					break;
				}
			}
			bool operator==(const JoystickButtonEventParameters& other) const
			{
				return buttonIndex == other.buttonIndex && joystickGuid == other.joystickGuid;
			}
			JoystickGuid joystickGuid;
			uint8_t buttonIndex = 0;
		};

		struct JoystickAxisEventParameters
		{
			bool check(const JoystickAxisEvent& event) const
			{
				return axisIndex == event.axisIndex && joystickGuid == getJoystickGuid(event.joystickId);
			}
			void set(ICustomEvent& customEvent, const JoystickAxisEvent& event) const
			{
				customEvent.type = ICustomEvent::Type::Int;
				customEvent.value.i = event.axisState;
			}
			bool operator==(const JoystickAxisEventParameters& other) const
			{
				return axisIndex == other.axisIndex && joystickGuid == other.joystickGuid;
			}
			JoystickGuid joystickGuid;
			uint8_t axisIndex = 0;
		};

		struct JoystickHatEventParameters
		{
			bool check(const JoystickHatEvent& event) const
			{
				return hatIndex == event.hatIndex && joystickGuid == getJoystickGuid(event.joystickId);
			}
			void set(ICustomEvent& customEvent, const JoystickHatEvent& event) const
			{
				customEvent.type = ICustomEvent::Type::Int;
				customEvent.value.i = int(event.joystickHatState);
			}
			bool operator==(const JoystickHatEventParameters& other) const
			{
				return hatIndex == other.hatIndex && joystickGuid == other.joystickGuid;
			}
			JoystickGuid joystickGuid;
			uint8_t hatIndex = 0;
		};

		struct CustomEventParameters
		{
			bool operator==(const CustomEventParameters& other) const;

			std::string toString() const;
			// Returns a shorter string, better suited for texts in the UI
			std::string toShortString() const;

			EventType eventType = EventType::none;
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