#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParametersUtilityFunctions.h"

#include "SpehsEngine/Input/EventUtilityFunctions.h"
#include "SpehsEngine/Input/JoystickUtilityFunctions.h"


namespace se
{
	Archive writeToArchive(const input::KeyboardEventParameters& eventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, eventParameters.key);
		return archive;
	}
	Archive writeToArchive(const input::MouseButtonEventParameters& eventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, eventParameters.mouseButton);
		return archive;
	}
	Archive writeToArchive(const input::MouseMotionEventParameters& eventParameters)
	{
		Archive archive;
		return archive;
	}
	Archive writeToArchive(const input::MouseWheelEventParameters& eventParameters)
	{
		Archive archive;
		return archive;
	}
	Archive writeToArchive(const input::MouseHoverEventParameters& eventParameters)
	{
		Archive archive;
		return archive;
	}
	Archive writeToArchive(const input::JoystickButtonEventParameters& eventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, eventParameters.joystickGuid);
		se_write_to_archive(archive, eventParameters.buttonIndex);
		return archive;
	}
	Archive writeToArchive(const input::JoystickAxisEventParameters& eventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, eventParameters.joystickGuid);
		se_write_to_archive(archive, eventParameters.axisIndex);
		return archive;
	}
	Archive writeToArchive(const input::JoystickHatEventParameters& eventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, eventParameters.joystickGuid);
		se_write_to_archive(archive, eventParameters.hatIndex);
		return archive;
	}

	bool readFromArchive(const Archive& archive, input::KeyboardEventParameters& eventParameters)
	{
		se_read_from_archive(archive, eventParameters.key);
		return true;
	}
	bool readFromArchive(const Archive& archive, input::MouseButtonEventParameters& eventParameters)
	{
		se_read_from_archive(archive, eventParameters.mouseButton);
		return true;
	}
	bool readFromArchive(const Archive& archive, input::MouseMotionEventParameters& eventParameters)
	{
		return true;
	}
	bool readFromArchive(const Archive& archive, input::MouseWheelEventParameters& eventParameters)
	{
		return true;
	}
	bool readFromArchive(const Archive& archive, input::MouseHoverEventParameters& eventParameters)
	{
		return true;
	}
	bool readFromArchive(const Archive& archive, input::JoystickButtonEventParameters& eventParameters)
	{
		se_read_from_archive(archive, eventParameters.joystickGuid);
		se_read_from_archive(archive, eventParameters.buttonIndex);
		return true;
	}
	bool readFromArchive(const Archive& archive, input::JoystickAxisEventParameters& eventParameters)
	{
		se_read_from_archive(archive, eventParameters.joystickGuid);
		se_read_from_archive(archive, eventParameters.axisIndex);
		return true;
	}
	bool readFromArchive(const Archive& archive, input::JoystickHatEventParameters& eventParameters)
	{
		se_read_from_archive(archive, eventParameters.joystickGuid);
		se_read_from_archive(archive, eventParameters.hatIndex);
		return true;
	}

	Archive writeToArchive(const input::CustomEventParameters& customEventParameters)
	{
	#define why_does_se_write_to_archive_not_work(__archive, __params) __archive.write(#__params, writeToArchive(__params))

		Archive archive;
		se_write_to_archive(archive, customEventParameters.eventType);
		switch (customEventParameters.eventType)
		{
		case input::EventType::none: break;
		case input::EventType::quit: break;
		case input::EventType::textInput: break;
		case input::EventType::fileDrop: break;
		case input::EventType::keyboard: why_does_se_write_to_archive_not_work(archive, customEventParameters.keyboardEventParameters); break;
		case input::EventType::mouseButton: why_does_se_write_to_archive_not_work(archive, customEventParameters.mouseButtonEventParameters); break;
		case input::EventType::mouseMotion: why_does_se_write_to_archive_not_work(archive, customEventParameters.mouseMotionEventParameters); break;
		case input::EventType::mouseWheel: why_does_se_write_to_archive_not_work(archive, customEventParameters.mouseWheelEventParameters); break;
		case input::EventType::mouseHover: why_does_se_write_to_archive_not_work(archive, customEventParameters.mouseHoverEventParameters); break;
		case input::EventType::joystickButton: why_does_se_write_to_archive_not_work(archive, customEventParameters.joystickButtonEventParameters); break;
		case input::EventType::joystickAxis: why_does_se_write_to_archive_not_work(archive, customEventParameters.joystickAxisEventParameters); break;
		case input::EventType::joystickHat: why_does_se_write_to_archive_not_work(archive, customEventParameters.joystickHatEventParameters); break;
		}
		return archive;
	}

	bool readFromArchive(const Archive& archive, input::CustomEventParameters& customEventParameters)
	{
		se_read_from_archive(archive, customEventParameters.eventType);
		switch (customEventParameters.eventType)
		{
		case input::EventType::none: break;
		case input::EventType::quit: break;
		case input::EventType::textInput: break;
		case input::EventType::fileDrop: break;
		case input::EventType::keyboard: se_read_from_archive(archive, customEventParameters.keyboardEventParameters); break;
		case input::EventType::mouseButton: se_read_from_archive(archive, customEventParameters.mouseButtonEventParameters); break;
		case input::EventType::mouseMotion: se_read_from_archive(archive, customEventParameters.mouseMotionEventParameters); break;
		case input::EventType::mouseWheel: se_read_from_archive(archive, customEventParameters.mouseWheelEventParameters); break;
		case input::EventType::mouseHover: se_read_from_archive(archive, customEventParameters.mouseHoverEventParameters); break;
		case input::EventType::joystickButton: se_read_from_archive(archive, customEventParameters.joystickButtonEventParameters); break;
		case input::EventType::joystickAxis: se_read_from_archive(archive, customEventParameters.joystickAxisEventParameters); break;
		case input::EventType::joystickHat: se_read_from_archive(archive, customEventParameters.joystickHatEventParameters); break;
		}
		return true;
	}
}
