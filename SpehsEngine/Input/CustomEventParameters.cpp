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
			case se::input::EventType::keyboardPress: if (!(keyboardPressEvent == other.keyboardPressEvent)) return false; break;
			case se::input::EventType::keyboardDown: if (!(keyboardDownEvent == other.keyboardDownEvent)) return false; break;
			case se::input::EventType::keyboardRelease: if (!(keyboardReleaseEvent == other.keyboardReleaseEvent)) return false; break;
			case se::input::EventType::textInput: if (!(textInputEvent == other.textInputEvent)) return false; break;
			case se::input::EventType::mouseButtonPress: if (!(mouseButtonPressEvent == other.mouseButtonPressEvent)) return false; break;
			case se::input::EventType::mouseButtonDown: if (!(mouseButtonDownEvent == other.mouseButtonDownEvent)) return false; break;
			case se::input::EventType::mouseButtonRelease: if (!(mouseButtonReleaseEvent == other.mouseButtonReleaseEvent)) return false; break;
			case se::input::EventType::mouseMotion: if (!(mouseMotionEvent == other.mouseMotionEvent)) return false; break;
			case se::input::EventType::mouseWheel: if (!(mouseWheelEvent == other.mouseWheelEvent)) return false; break;
			case se::input::EventType::mouseHover: if (!(mouseHoverEvent == other.mouseHoverEvent)) return false; break;
			case se::input::EventType::joystickButtonPress: if (!(joystickButtonPressEvent == other.joystickButtonPressEvent)) return false; break;
			case se::input::EventType::joystickButtonDown: if (!(joystickButtonDownEvent == other.joystickButtonDownEvent)) return false; break;
			case se::input::EventType::joystickButtonRelease: if (!(joystickButtonReleaseEvent == other.joystickButtonReleaseEvent)) return false; break;
			case se::input::EventType::joystickAxis: if (!(joystickAxisEvent == other.joystickAxisEvent)) return false; break;
			case se::input::EventType::quit: if (!(quitEvent == other.quitEvent)) return false; break;
			case se::input::EventType::fileDrop: if (!(fileDropEvent == other.fileDropEvent)) return false; break;
			}

			return true;
		}

		std::string CustomEventParameters::toString() const
		{
			switch (eventType)
			{
			case se::input::EventType::none: return "None";
			case se::input::EventType::keyboardPress: return "Key: " + std::string(se::input::toString(keyboardPressEvent.key));
			case se::input::EventType::keyboardDown: return "Key hold: " + std::string(se::input::toString(keyboardDownEvent.key));
			case se::input::EventType::keyboardRelease: return "Key release: " + std::string(se::input::toString(keyboardReleaseEvent.key));
			case se::input::EventType::textInput: return "Text input: " + textInputEvent.buffer;
			case se::input::EventType::mouseButtonPress: return "Mouse button: " + std::string(se::input::toString(mouseButtonPressEvent.button));
			case se::input::EventType::mouseButtonDown: return "Mouse button hold: " + std::string(se::input::toString(mouseButtonDownEvent.button));
			case se::input::EventType::mouseButtonRelease: return "Mouse button release: " + std::string(se::input::toString(mouseButtonReleaseEvent.button));
			case se::input::EventType::mouseMotion: return "Mouse motion";
			case se::input::EventType::mouseWheel: return "Mouse wheel: " + std::to_string(mouseWheelEvent.delta.y);
			case se::input::EventType::mouseHover: return "Mouse hover";
			case se::input::EventType::joystickButtonPress: return "Joystick button: " + std::to_string(joystickButtonPressEvent.button);
			case se::input::EventType::joystickButtonDown: return "Joystick button hold: " + std::to_string(joystickButtonDownEvent.button);
			case se::input::EventType::joystickButtonRelease: return "Joystick button release: " + std::to_string(joystickButtonReleaseEvent.button);
			case se::input::EventType::joystickAxis: return "Joystick axis: " + std::to_string(joystickAxisEvent.axisIndex);
			case se::input::EventType::quit: return "Quit";
			case se::input::EventType::fileDrop: return "Filedrop: " + fileDropEvent.filepath;
			}

			return "Error";
		}
	}
}
