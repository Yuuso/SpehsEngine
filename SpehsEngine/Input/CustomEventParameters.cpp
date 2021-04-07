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
	}
}
