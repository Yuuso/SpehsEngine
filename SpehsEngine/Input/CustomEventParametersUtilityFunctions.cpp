#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParametersUtilityFunctions.h"

#include "SpehsEngine/Input/EventUtilityFunctions.h"


namespace se
{
	Archive writeToArchive(const input::CustomEventParameters& customEventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, customEventParameters.eventType);
		switch (customEventParameters.eventType)
		{
		case input::EventType::none: break;
		case input::EventType::keyboardPress: se_write_to_archive(archive, customEventParameters.keyboardPressEvent); break;
		case input::EventType::keyboardDown: se_write_to_archive(archive, customEventParameters.keyboardDownEvent); break;
		case input::EventType::keyboardRelease: se_write_to_archive(archive, customEventParameters.keyboardReleaseEvent); break;
		case input::EventType::textInput: se_write_to_archive(archive, customEventParameters.textInputEvent); break;
		case input::EventType::mouseButtonPress: se_write_to_archive(archive, customEventParameters.mouseButtonPressEvent); break;
		case input::EventType::mouseButtonDown: se_write_to_archive(archive, customEventParameters.mouseButtonDownEvent); break;
		case input::EventType::mouseButtonRelease: se_write_to_archive(archive, customEventParameters.mouseButtonReleaseEvent); break;
		case input::EventType::mouseMotion: se_write_to_archive(archive, customEventParameters.mouseMotionEvent); break;
		case input::EventType::mouseWheel: se_write_to_archive(archive, customEventParameters.mouseWheelEvent); break;
		case input::EventType::mouseHover: se_write_to_archive(archive, customEventParameters.mouseHoverEvent); break;
		case input::EventType::joystickButtonPress: se_write_to_archive(archive, customEventParameters.joystickButtonPressEvent); break;
		case input::EventType::joystickButtonDown: se_write_to_archive(archive, customEventParameters.joystickButtonDownEvent); break;
		case input::EventType::joystickButtonRelease: se_write_to_archive(archive, customEventParameters.joystickButtonReleaseEvent); break;
		case input::EventType::joystickAxis: se_write_to_archive(archive, customEventParameters.joystickAxisEvent); break;
		case input::EventType::quit: se_write_to_archive(archive, customEventParameters.quitEvent); break;
		case input::EventType::fileDrop: se_write_to_archive(archive, customEventParameters.fileDropEvent); break;
		}
		return archive;
	}

	bool readFromArchive(const Archive& archive, input::CustomEventParameters& customEventParameters)
	{
		se_read_from_archive(archive, customEventParameters.eventType);
		switch (customEventParameters.eventType)
		{
		case input::EventType::none: break;
		case input::EventType::keyboardPress: se_read_from_archive(archive, customEventParameters.keyboardPressEvent); break;
		case input::EventType::keyboardDown: se_read_from_archive(archive, customEventParameters.keyboardDownEvent); break;
		case input::EventType::keyboardRelease: se_read_from_archive(archive, customEventParameters.keyboardReleaseEvent); break;
		case input::EventType::textInput: se_read_from_archive(archive, customEventParameters.textInputEvent); break;
		case input::EventType::mouseButtonPress: se_read_from_archive(archive, customEventParameters.mouseButtonPressEvent); break;
		case input::EventType::mouseButtonDown: se_read_from_archive(archive, customEventParameters.mouseButtonDownEvent); break;
		case input::EventType::mouseButtonRelease: se_read_from_archive(archive, customEventParameters.mouseButtonReleaseEvent); break;
		case input::EventType::mouseMotion: se_read_from_archive(archive, customEventParameters.mouseMotionEvent); break;
		case input::EventType::mouseWheel: se_read_from_archive(archive, customEventParameters.mouseWheelEvent); break;
		case input::EventType::mouseHover: se_read_from_archive(archive, customEventParameters.mouseHoverEvent); break;
		case input::EventType::joystickButtonPress: se_read_from_archive(archive, customEventParameters.joystickButtonPressEvent); break;
		case input::EventType::joystickButtonDown: se_read_from_archive(archive, customEventParameters.joystickButtonDownEvent); break;
		case input::EventType::joystickButtonRelease: se_read_from_archive(archive, customEventParameters.joystickButtonReleaseEvent); break;
		case input::EventType::joystickAxis: se_read_from_archive(archive, customEventParameters.joystickAxisEvent); break;
		case input::EventType::quit: se_read_from_archive(archive, customEventParameters.quitEvent); break;
		case input::EventType::fileDrop: se_read_from_archive(archive, customEventParameters.fileDropEvent); break;
		}
		return true;
	}
}
