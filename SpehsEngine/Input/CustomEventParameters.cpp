#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParameters.h"

#include "SpehsEngine/Input/JoystickUtilityFunctions.h"


namespace se
{
	namespace input
	{
		bool KeyboardEventParameters::check(const KeyboardEvent& event) const
		{
			return event.key == key;
		}

		void KeyboardEventParameters::set(ICustomEvent& customEvent, const KeyboardEvent& event) const
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

		bool KeyboardEventParameters::operator==(const KeyboardEventParameters& other) const
		{
			return key == other.key;
		}

		bool MouseButtonEventParameters::check(const MouseButtonEvent& event) const
		{
			return event.button == mouseButton;
		}

		void MouseButtonEventParameters::set(ICustomEvent& customEvent, const MouseButtonEvent& event) const
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

		bool MouseButtonEventParameters::operator==(const MouseButtonEventParameters& other) const
		{
			return mouseButton == other.mouseButton;
		}

		bool MouseMotionEventParameters::check(const MouseMotionEvent&) const
		{
			return true;
		}

		void MouseMotionEventParameters::set(ICustomEvent& customEvent, const MouseMotionEvent& event) const
		{
			customEvent.type = ICustomEvent::Type::Vec2;
			customEvent.value.vec2 = event.translation;
		}

		bool MouseMotionEventParameters::operator==(const MouseMotionEventParameters&) const
		{
			return true;
		}

		bool MouseWheelEventParameters::check(const MouseWheelEvent&) const
		{
			return true;
		}

		void MouseWheelEventParameters::set(ICustomEvent& customEvent, const MouseWheelEvent& event) const
		{
			customEvent.type = ICustomEvent::Type::Ivec2;
			customEvent.value.ivec2 = event.delta;
		}

		bool MouseWheelEventParameters::operator==(const MouseWheelEventParameters&) const
		{
			return true;
		}

		bool MouseHoverEventParameters::check(const MouseHoverEvent&) const
		{
			return true;
		}

		void MouseHoverEventParameters::set(ICustomEvent& customEvent, const MouseHoverEvent& event) const
		{
			customEvent.type = ICustomEvent::Type::Vec2;
			customEvent.value.vec2 = event.position;
		}

		bool MouseHoverEventParameters::operator==(const MouseHoverEventParameters&) const
		{
			return true;
		}

		bool JoystickButtonEventParameters::check(const JoystickButtonEvent& event) const
		{
			return buttonIndex == event.buttonIndex && joystickGuid == getJoystickGuid(event.joystickId);
		}

		void JoystickButtonEventParameters::set(ICustomEvent& customEvent, const JoystickButtonEvent& event) const
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

		bool JoystickButtonEventParameters::operator==(const JoystickButtonEventParameters& other) const
		{
			return buttonIndex == other.buttonIndex && joystickGuid == other.joystickGuid;
		}

		bool JoystickAxisEventParameters::check(const JoystickAxisEvent& event) const
		{
			return axisIndex == event.axisIndex && joystickGuid == getJoystickGuid(event.joystickId);
		}

		void JoystickAxisEventParameters::set(ICustomEvent& customEvent, const JoystickAxisEvent& event) const
		{
			customEvent.type = ICustomEvent::Type::Int;
			customEvent.value.i = event.axisState;
		}

		bool JoystickAxisEventParameters::operator==(const JoystickAxisEventParameters& other) const
		{
			return axisIndex == other.axisIndex && joystickGuid == other.joystickGuid;
		}

		bool JoystickHatEventParameters::check(const JoystickHatEvent& event) const
		{
			return hatIndex == event.hatIndex && joystickGuid == getJoystickGuid(event.joystickId);
		}

		void JoystickHatEventParameters::set(ICustomEvent& customEvent, const JoystickHatEvent& event) const
		{
			customEvent.type = ICustomEvent::Type::Int;
			customEvent.value.i = int(event.joystickHatState);
		}

		bool JoystickHatEventParameters::operator==(const JoystickHatEventParameters& other) const
		{
			return hatIndex == other.hatIndex && joystickGuid == other.joystickGuid;
		}

		bool CustomEventParameters::operator==(const CustomEventParameters& other) const
		{
			if (eventType != other.eventType)
			{
				return false;
			}

			switch (eventType)
			{
			case input::EventType::none: break;
			case input::EventType::quit: break;
			case input::EventType::textInput: break;
			case input::EventType::fileDrop: break;
			case input::EventType::keyboard: if (!(keyboardEventParameters == other.keyboardEventParameters)) return false; break;
			case input::EventType::mouseButton: if (!(mouseButtonEventParameters == other.mouseButtonEventParameters)) return false; break;
			case input::EventType::mouseMotion: if (!(mouseMotionEventParameters == other.mouseMotionEventParameters)) return false; break;
			case input::EventType::mouseWheel: if (!(mouseWheelEventParameters == other.mouseWheelEventParameters)) return false; break;
			case input::EventType::mouseHover: if (!(mouseHoverEventParameters == other.mouseHoverEventParameters)) return false; break;
			case input::EventType::joystickButton: if (!(joystickButtonEventParameters == other.joystickButtonEventParameters)) return false; break;
			case input::EventType::joystickAxis: if (!(joystickAxisEventParameters == other.joystickAxisEventParameters)) return false; break;
			case input::EventType::joystickHat: if (!(joystickHatEventParameters == other.joystickHatEventParameters)) return false; break;
			}

			return true;
		}

		std::string CustomEventParameters::toString() const
		{
			switch (eventType)
			{
			case EventType::none: return "None";
			case EventType::quit: break;
			case EventType::textInput: break;
			case EventType::fileDrop: break;
			case EventType::keyboard: return "Key: " + std::string(input::toString(keyboardEventParameters.key));
			case EventType::mouseButton: return "Mouse button: " + std::string(input::toString(mouseButtonEventParameters.mouseButton));
			case EventType::mouseMotion: return "Mouse motion";
			case EventType::mouseWheel: return "Mouse wheel";
			case EventType::mouseHover: return "Mouse hover";
			case EventType::joystickButton: return "Joystick button: " + std::to_string(joystickButtonEventParameters.buttonIndex);
			case EventType::joystickAxis: return "Joystick axis: " + std::to_string(joystickAxisEventParameters.axisIndex);
			case EventType::joystickHat: return "Joystick hat: " + std::to_string(joystickHatEventParameters.hatIndex);
			}

			return "Error";
		}

		std::string CustomEventParameters::toShortString() const
		{
			switch (eventType)
			{
			case EventType::none: return "none";
			case EventType::quit: break;
			case EventType::textInput: break;
			case EventType::fileDrop: break;
			case EventType::keyboard: return std::string(input::toShortString(keyboardEventParameters.key));
			case EventType::mouseButton: return std::string(input::toShortString(mouseButtonEventParameters.mouseButton));
			case EventType::mouseMotion: return "Momo";
			case EventType::mouseWheel: return "Mowh";
			case EventType::mouseHover: return "Moho";
			case EventType::joystickButton: return "Jsb" + std::to_string(joystickButtonEventParameters.buttonIndex);
			case EventType::joystickAxis: return "Jsa" + std::to_string(joystickAxisEventParameters.axisIndex);
			case EventType::joystickHat: return "Jsa" + std::to_string(joystickHatEventParameters.hatIndex);
			}

			return "Error";
		}
	}
}
