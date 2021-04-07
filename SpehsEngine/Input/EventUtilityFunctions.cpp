#include "stdafx.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"

#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/Math/GLMVectorUtilityFunctions.h"


namespace se
{
	Archive writeToArchive(const input::KeyboardPressEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.key);
		se_write_to_archive(archive, event.scancode);
		return archive;
	}

	Archive writeToArchive(const input::KeyboardDownEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.key);
		return archive;
	}

	Archive writeToArchive(const input::KeyboardReleaseEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.key);
		se_write_to_archive(archive, event.scancode);
		return archive;
	}

	Archive writeToArchive(const input::TextInputEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.buffer);
		return archive;
	}

	Archive writeToArchive(const input::MouseButtonPressEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.button);
		return archive;
	}

	Archive writeToArchive(const input::MouseButtonDownEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.button);
		return archive;
	}

	Archive writeToArchive(const input::MouseButtonReleaseEvent& event)
	{
		Archive archive;
		se_write_to_archive(archive, event.button);
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

	Archive writeToArchive(const input::JoystickButtonPressEvent& event)
	{
		Archive archive;
		// TODO: joystick pointer to joystick id
		se_write_to_archive(archive, event.button);
		return archive;
	}

	Archive writeToArchive(const input::JoystickButtonDownEvent& event)
	{
		Archive archive;
		// TODO: joystick pointer to joystick id
		se_write_to_archive(archive, event.button);
		return archive;
	}

	Archive writeToArchive(const input::JoystickButtonReleaseEvent& event)
	{
		Archive archive;
		// TODO: joystick pointer to joystick id
		se_write_to_archive(archive, event.button);
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


	bool readFromArchive(const Archive& archive, input::KeyboardPressEvent& event)
	{
		se_read_from_archive(archive, event.key);
		se_read_from_archive(archive, event.scancode);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::KeyboardDownEvent& event)
	{
		se_read_from_archive(archive, event.key);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::KeyboardReleaseEvent& event)
	{
		se_read_from_archive(archive, event.key);
		se_read_from_archive(archive, event.scancode);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::TextInputEvent& event)
	{
		se_read_from_archive(archive, event.buffer);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseButtonPressEvent& event)
	{
		se_read_from_archive(archive, event.button);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseButtonDownEvent& event)
	{
		se_read_from_archive(archive, event.button);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::MouseButtonReleaseEvent& event)
	{
		se_read_from_archive(archive, event.button);
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

	bool readFromArchive(const Archive& archive, input::JoystickButtonPressEvent& event)
	{
		// TODO: joystick pointer to joystick id
		se_read_from_archive(archive, event.button);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::JoystickButtonDownEvent& event)
	{
		// TODO: joystick pointer to joystick id
		se_read_from_archive(archive, event.button);
		return true;
	}

	bool readFromArchive(const Archive& archive, input::JoystickButtonReleaseEvent& event)
	{
		// TODO: joystick pointer to joystick id
		se_read_from_archive(archive, event.button);
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
