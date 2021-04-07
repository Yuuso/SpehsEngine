#pragma once

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Core/Archive.h"


namespace se
{
	Archive writeToArchive(const input::KeyboardPressEvent& event);
	Archive writeToArchive(const input::KeyboardDownEvent& event);
	Archive writeToArchive(const input::KeyboardReleaseEvent& event);
	Archive writeToArchive(const input::TextInputEvent& event);
	Archive writeToArchive(const input::MouseButtonPressEvent& event);
	Archive writeToArchive(const input::MouseButtonDownEvent& event);
	Archive writeToArchive(const input::MouseButtonReleaseEvent& event);
	Archive writeToArchive(const input::MouseMotionEvent& event);
	Archive writeToArchive(const input::MouseWheelEvent& event);
	Archive writeToArchive(const input::MouseHoverEvent& event);
	Archive writeToArchive(const input::JoystickButtonPressEvent& event);
	Archive writeToArchive(const input::JoystickButtonDownEvent& event);
	Archive writeToArchive(const input::JoystickButtonReleaseEvent& event);
	Archive writeToArchive(const input::JoystickAxisEvent& event);
	Archive writeToArchive(const input::QuitEvent& event);
	Archive writeToArchive(const input::FileDropEvent& event);

	bool readFromArchive(const Archive& archive, input::KeyboardPressEvent& event);
	bool readFromArchive(const Archive& archive, input::KeyboardDownEvent& event);
	bool readFromArchive(const Archive& archive, input::KeyboardReleaseEvent& event);
	bool readFromArchive(const Archive& archive, input::TextInputEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseButtonPressEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseButtonDownEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseButtonReleaseEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseMotionEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseWheelEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseHoverEvent& event);
	bool readFromArchive(const Archive& archive, input::JoystickButtonPressEvent& event);
	bool readFromArchive(const Archive& archive, input::JoystickButtonDownEvent& event);
	bool readFromArchive(const Archive& archive, input::JoystickButtonReleaseEvent& event);
	bool readFromArchive(const Archive& archive, input::JoystickAxisEvent& event);
	bool readFromArchive(const Archive& archive, input::QuitEvent& event);
	bool readFromArchive(const Archive& archive, input::FileDropEvent& event);
}
