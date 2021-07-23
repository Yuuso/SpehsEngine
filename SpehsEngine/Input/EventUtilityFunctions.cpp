#include "stdafx.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"

#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Math/GLMVectorUtilityFunctions.h"


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
		// TODO: joystick pointer to joystick id
		se_write_to_archive(archive, event.button);
		se_write_to_archive(archive, event.type);
		return archive;
	}

	Archive writeToArchive(const input::JoystickAxisEvent& event)
	{
		Archive archive;
		// TODO: joystick pointer to joystick id
		se_write_to_archive(archive, event.axisIndex);
		se_write_to_archive(archive, event.axisMovement);
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
		// TODO: joystick pointer to joystick id
		se_read_from_archive(archive, event.button);
		se_read_from_archive(archive, event.type);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::JoystickAxisEvent& event)
	{
		// TODO: joystick pointer to joystick id
		se_read_from_archive(archive, event.axisIndex);
		se_read_from_archive(archive, event.axisMovement);
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
