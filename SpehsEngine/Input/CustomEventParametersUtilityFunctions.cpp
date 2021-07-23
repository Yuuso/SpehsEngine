#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParametersUtilityFunctions.h"

#include "SpehsEngine/Input/EventUtilityFunctions.h"


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
		se_write_to_archive(archive, eventParameters.button);
		return archive;
	}
	Archive writeToArchive(const input::JoystickAxisEventParameters& eventParameters)
	{
		Archive archive;
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
		se_read_from_archive(archive, eventParameters.button);
		return true;
	}
	bool readFromArchive(const Archive& archive, input::JoystickAxisEventParameters& eventParameters)
	{
		se_read_from_archive(archive, eventParameters.axisIndex);
		return true;
	}

	Archive writeToArchive(const input::CustomEventParameters& customEventParameters)
	{
		Archive archive;
		se_write_to_archive(archive, customEventParameters.eventType);
		switch (customEventParameters.eventType)
		{
		case input::EventType::none: break;
		case input::EventType::quit: break;
		case input::EventType::textInput: break;
		case input::EventType::fileDrop: break;
		case input::EventType::keyboard: se_write_to_archive(archive, customEventParameters.keyboardEventParameters); break;
		case input::EventType::mouseButton: se_write_to_archive(archive, customEventParameters.mouseButtonEventParameters); break;
		case input::EventType::mouseMotion: se_write_to_archive(archive, customEventParameters.mouseMotionEventParameters); break;
		case input::EventType::mouseWheel: se_write_to_archive(archive, customEventParameters.mouseWheelEventParameters); break;
		case input::EventType::mouseHover: se_write_to_archive(archive, customEventParameters.mouseHoverEventParameters); break;
		case input::EventType::joystickButton: se_write_to_archive(archive, customEventParameters.joystickButtonEventParameters); break;
		case input::EventType::joystickAxis: se_write_to_archive(archive, customEventParameters.joystickAxisEventParameters); break;
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
		}
		return true;
	}
}
