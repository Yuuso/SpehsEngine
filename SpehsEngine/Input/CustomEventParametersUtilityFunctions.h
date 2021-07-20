#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Input/CustomEventParameters.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"


namespace se
{
	Archive writeToArchive(const input::CustomEventParameters& customEventParameters);
	bool readFromArchive(const Archive& archive, input::CustomEventParameters& customEventParameters);
}

namespace std
{
	template<> struct hash<se::input::CustomEventParameters>
	{
		size_t operator()(const se::input::CustomEventParameters& customEventParameters) const
		{
			size_t data[2];
			data[0] = size_t(customEventParameters.eventType);
			switch (customEventParameters.eventType)
			{
			case se::input::EventType::none: break;
			case se::input::EventType::keyboardPress: data[1] = std::hash<se::input::KeyboardPressEvent>()(customEventParameters.keyboardPressEvent); break;
			case se::input::EventType::keyboardDown: data[1] = std::hash<se::input::KeyboardDownEvent>()(customEventParameters.keyboardDownEvent); break;
			case se::input::EventType::keyboardRelease: data[1] = std::hash<se::input::KeyboardReleaseEvent>()(customEventParameters.keyboardReleaseEvent); break;
			case se::input::EventType::textInput: data[1] = std::hash<se::input::TextInputEvent>()(customEventParameters.textInputEvent); break;
			case se::input::EventType::mouseButtonPress: data[1] = std::hash<se::input::MouseButtonPressEvent>()(customEventParameters.mouseButtonPressEvent); break;
			case se::input::EventType::mouseButtonDown: data[1] = std::hash<se::input::MouseButtonDownEvent>()(customEventParameters.mouseButtonDownEvent); break;
			case se::input::EventType::mouseButtonRelease: data[1] = std::hash<se::input::MouseButtonReleaseEvent>()(customEventParameters.mouseButtonReleaseEvent); break;
			case se::input::EventType::mouseMotion: data[1] = std::hash<se::input::MouseMotionEvent>()(customEventParameters.mouseMotionEvent); break;
			case se::input::EventType::mouseWheel: data[1] = std::hash<se::input::MouseWheelEvent>()(customEventParameters.mouseWheelEvent); break;
			case se::input::EventType::mouseHover: data[1] = std::hash<se::input::MouseHoverEvent>()(customEventParameters.mouseHoverEvent); break;
			case se::input::EventType::joystickButtonPress: data[1] = std::hash<se::input::JoystickButtonPressEvent>()(customEventParameters.joystickButtonPressEvent); break;
			case se::input::EventType::joystickButtonDown: data[1] = std::hash<se::input::JoystickButtonDownEvent>()(customEventParameters.joystickButtonDownEvent); break;
			case se::input::EventType::joystickButtonRelease: data[1] = std::hash<se::input::JoystickButtonReleaseEvent>()(customEventParameters.joystickButtonReleaseEvent); break;
			case se::input::EventType::joystickAxis: data[1] = std::hash<se::input::JoystickAxisEvent>()(customEventParameters.joystickAxisEvent); break;
			case se::input::EventType::quit: data[1] = std::hash<se::input::QuitEvent>()(customEventParameters.quitEvent); break;
			case se::input::EventType::fileDrop: data[1] = std::hash<se::input::FileDropEvent>()(customEventParameters.fileDropEvent); break;
			}
			return size_t(se::murmurHash3_x86_32(data, sizeof(size_t) * 2, 0));
		}
	};
}
