#include "stdafx.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"

#include "SpehsEngine/Input/JoystickUtilityFunctions.h"


namespace se
{
	Archive writeToArchive(const input::KeyboardEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.key);
		se_write_to_archive(archive, event.scancode);
		se_write_to_archive(archive, event.type);
		return archive;
	}

	Archive writeToArchive(const input::TextInputEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.buffer);
		return archive;
	}

	Archive writeToArchive(const input::MouseButtonEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.button);
		se_write_to_archive(archive, event.type);
		return archive;
	}

	Archive writeToArchive(const input::MouseMotionEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.position);
		se_write_to_archive(archive, event.previousPosition);
		se_write_to_archive(archive, event.translation);
		return archive;
	}

	Archive writeToArchive(const input::MouseWheelEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.delta);
		return archive;
	}

	Archive writeToArchive(const input::MouseHoverEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.position);
		return archive;
	}

	Archive writeToArchive(const input::JoystickButtonEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.joystickId);
		se_write_to_archive(archive, event.joystickGuid);
		se_write_to_archive(archive, event.buttonIndex);
		se_write_to_archive(archive, event.type);
		return archive;
	}

	Archive writeToArchive(const input::JoystickAxisEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.joystickId);
		se_write_to_archive(archive, event.joystickGuid);
		se_write_to_archive(archive, event.axisIndex);
		se_write_to_archive(archive, event.axisState);
		return archive;
	}

	Archive writeToArchive(const input::JoystickHatEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.joystickId);
		se_write_to_archive(archive, event.joystickGuid);
		se_write_to_archive(archive, event.hatIndex);
		se_write_to_archive(archive, event.joystickHatState);
		return archive;
	}

	Archive writeToArchive(const input::QuitEvent& event)
	{
		Archive archive;
		return archive;
	}

	Archive writeToArchive(const input::FileDropEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.filepath);
		return archive;
	}


	bool readFromArchive(const Archive& archive, input::KeyboardEvent& event)
	{
		se_read_from_archive(archive, event.key);
		se_read_from_archive(archive, event.scancode);
		se_read_from_archive(archive, event.type);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::TextInputEvent& event)
	{
		se_read_from_archive(archive, event.buffer);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseButtonEvent& event)
	{
		se_read_from_archive(archive, event.button);
		se_read_from_archive(archive, event.type);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseMotionEvent& event)
	{
		se_read_from_archive(archive, event.position);
		se_read_from_archive(archive, event.previousPosition);
		se_read_from_archive(archive, event.translation);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseWheelEvent& event)
	{
		se_read_from_archive(archive, event.delta);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseHoverEvent& event)
	{
		se_read_from_archive(archive, event.position);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::JoystickButtonEvent& event)
	{
		se_read_from_archive(archive, event.joystickId);
		se_read_from_archive(archive, event.joystickGuid);
		se_read_from_archive(archive, event.buttonIndex);
		se_read_from_archive(archive, event.type);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::JoystickAxisEvent& event)
	{
		se_read_from_archive(archive, event.joystickId);
		se_read_from_archive(archive, event.joystickGuid);
		se_read_from_archive(archive, event.axisIndex);
		se_read_from_archive(archive, event.axisState);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::JoystickHatEvent& event)
	{
		se_read_from_archive(archive, event.joystickId);
		se_read_from_archive(archive, event.joystickGuid);
		se_read_from_archive(archive, event.hatIndex);
		se_read_from_archive(archive, event.joystickHatState);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::QuitEvent& event)
	{
		return true;
	}

	bool readFromArchive(const Archive& archive, input::FileDropEvent& event)
	{
		se_read_from_archive(archive, event.filepath);
		return true;
	}
}
