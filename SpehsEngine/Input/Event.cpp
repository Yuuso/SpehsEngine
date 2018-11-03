#include "stdafx.h"
#include "SpehsEngine/Input/Event.h"

namespace se
{
	namespace input
	{
		std::string toString(const EventType eventType)
		{
			switch (eventType)
			{
			default: se_assert(false && "unknown event type"); return "error";
			case EventType::none: return "none";
			case EventType::keyboardPress: return "keyboardPress";
			case EventType::keyboardDown: return "keyboardDown";
			case EventType::keyboardRelease: return "keyboardRelease";
			case EventType::mouseButtonPress: return "mouseButtonPress";
			case EventType::mouseButtonDown: return "mouseButtonDown";
			case EventType::mouseButtonRelease: return "mouseButtonRelease";
			case EventType::mouseMotion: return "mouseMotion";
			case EventType::mouseWheel: return "mouseWheel";
			case EventType::mouseHover: return "mouseHover";
			case EventType::joystickButtonPress: return "joystickButtonPress";
			case EventType::joystickButtonDown: return "joystickButtonDown";
			case EventType::joystickButtonRelease: return "joystickButtonRelease";
			case EventType::joystickAxis: return "joystickAxis";
			case EventType::quit: return "quit";
			case EventType::fileDrop: return "fileDrop";
			}
		}
	}
}
