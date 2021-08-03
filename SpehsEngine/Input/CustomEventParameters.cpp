#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParameters.h"


namespace se
{
	namespace input
	{
		bool CustomEventParameters::operator==(const CustomEventParameters& other) const
		{
			if (eventType != other.eventType)
			{
				return false;
			}

			switch (eventType)
			{
			case se::input::EventType::none: break;
			case se::input::EventType::quit: break;
			case se::input::EventType::textInput: break;
			case se::input::EventType::fileDrop: break;
			case se::input::EventType::keyboard: if (!(keyboardEventParameters == other.keyboardEventParameters)) return false; break;
			case se::input::EventType::mouseButton: if (!(mouseButtonEventParameters == other.mouseButtonEventParameters)) return false; break;
			case se::input::EventType::mouseMotion: if (!(mouseMotionEventParameters == other.mouseMotionEventParameters)) return false; break;
			case se::input::EventType::mouseWheel: if (!(mouseWheelEventParameters == other.mouseWheelEventParameters)) return false; break;
			case se::input::EventType::mouseHover: if (!(mouseHoverEventParameters == other.mouseHoverEventParameters)) return false; break;
			case se::input::EventType::joystickButton: if (!(joystickButtonEventParameters == other.joystickButtonEventParameters)) return false; break;
			case se::input::EventType::joystickAxis: if (!(joystickAxisEventParameters == other.joystickAxisEventParameters)) return false; break;
			case se::input::EventType::joystickHat: if (!(joystickHatEventParameters == other.joystickHatEventParameters)) return false; break;
			}

			return true;
		}

		std::string CustomEventParameters::toString() const
		{
			switch (eventType)
			{
			case se::input::EventType::none: return "None";
			case se::input::EventType::quit: break;
			case se::input::EventType::textInput: break;
			case se::input::EventType::fileDrop: break;
			case se::input::EventType::keyboard: return "Key: " + std::string(se::input::toString(keyboardEventParameters.key));
			case se::input::EventType::mouseButton: return "Mouse button: " + std::string(se::input::toString(mouseButtonEventParameters.mouseButton));
			case se::input::EventType::mouseMotion: return "Mouse motion";
			case se::input::EventType::mouseWheel: return "Mouse wheel";
			case se::input::EventType::mouseHover: return "Mouse hover";
			case se::input::EventType::joystickButton: return "Joystick button: " + std::to_string(joystickButtonEventParameters.buttonIndex);
			case se::input::EventType::joystickAxis: return "Joystick axis: " + std::to_string(joystickAxisEventParameters.axisIndex);
			case se::input::EventType::joystickHat: return "Joystick hat: " + std::to_string(joystickHatEventParameters.hatIndex);
			}

			return "Error";
		}

		std::string CustomEventParameters::toShortString() const
		{
			switch (eventType)
			{
			case se::input::EventType::none: return "none";
			case se::input::EventType::quit: break;
			case se::input::EventType::textInput: break;
			case se::input::EventType::fileDrop: break;
			case se::input::EventType::keyboard: return std::string(se::input::toShortString(keyboardEventParameters.key));
			case se::input::EventType::mouseButton: return std::string(se::input::toShortString(mouseButtonEventParameters.mouseButton));
			case se::input::EventType::mouseMotion: return "Momo";
			case se::input::EventType::mouseWheel: return "Mowh";
			case se::input::EventType::mouseHover: return "Moho";
			case se::input::EventType::joystickButton: return "Jsb" + std::to_string(joystickButtonEventParameters.buttonIndex);
			case se::input::EventType::joystickAxis: return "Jsa" + std::to_string(joystickAxisEventParameters.axisIndex);
			case se::input::EventType::joystickHat: return "Jsa" + std::to_string(joystickHatEventParameters.hatIndex);
			}

			return "Error";
		}
	}
}
