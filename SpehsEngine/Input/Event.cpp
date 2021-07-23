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
			case EventType::keyboard: return "keyboard";
			case EventType::textInput: return "textInput";
			case EventType::mouseButton: return "mouseButton";
			case EventType::mouseMotion: return "mouseMotion";
			case EventType::mouseWheel: return "mouseWheel";
			case EventType::mouseHover: return "mouseHover";
			case EventType::joystickButton: return "joystickButton";
			case EventType::joystickAxis: return "joystickAxis";
			case EventType::quit: return "quit";
			case EventType::fileDrop: return "fileDrop";
			}
		}
	}
}
