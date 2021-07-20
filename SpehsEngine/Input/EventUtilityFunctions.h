#pragma once

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/Murmur3.h"


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

namespace std
{
	template<> struct hash<se::input::KeyboardPressEvent>
	{
		size_t operator()(const se::input::KeyboardPressEvent& event) const
		{
			return size_t(event.key);
		}
	};
	template<> struct hash<se::input::KeyboardDownEvent>
	{
		size_t operator()(const se::input::KeyboardDownEvent& event) const
		{
			return size_t(event.key);
		}
	};
	template<> struct hash<se::input::KeyboardReleaseEvent>
	{
		size_t operator()(const se::input::KeyboardReleaseEvent& event) const
		{
			return size_t(event.key);
		}
	};
	template<> struct hash<se::input::TextInputEvent>
	{
		size_t operator()(const se::input::TextInputEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(event.buffer.data(), event.buffer.size(), 0));
		}
	};
	template<> struct hash<se::input::MouseButtonPressEvent>
	{
		size_t operator()(const se::input::MouseButtonPressEvent& event) const
		{
			return size_t(event.button);
		}
	};
	template<> struct hash<se::input::MouseButtonDownEvent>
	{
		size_t operator()(const se::input::MouseButtonDownEvent& event) const
		{
			return size_t(event.button);
		}
	};
	template<> struct hash<se::input::MouseButtonReleaseEvent>
	{
		size_t operator()(const se::input::MouseButtonReleaseEvent& event) const
		{
			return size_t(event.button);
		}
	};
	template<> struct hash<se::input::MouseMotionEvent>
	{
		size_t operator()(const se::input::MouseMotionEvent& event) const
		{
			static_assert(sizeof(event) == 24);
			return size_t(se::murmurHash3_x86_32(&event, sizeof(event), 0));
		}
	};
	template<> struct hash<se::input::MouseWheelEvent>
	{
		size_t operator()(const se::input::MouseWheelEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(&event.delta, 8, 0));
		}
	};
	template<> struct hash<se::input::MouseHoverEvent>
	{
		size_t operator()(const se::input::MouseHoverEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(&event.position, 8, 0));
		}
	};
	template<> struct hash<se::input::JoystickButtonPressEvent>
	{
		size_t operator()(const se::input::JoystickButtonPressEvent& event) const
		{
			se_assert(false && "TODO joystick pointer");
			return size_t(event.button);
		}
	};
	template<> struct hash<se::input::JoystickButtonDownEvent>
	{
		size_t operator()(const se::input::JoystickButtonDownEvent& event) const
		{
			se_assert(false && "TODO joystick pointer");
			return size_t(event.button);
		}
	};
	template<> struct hash<se::input::JoystickButtonReleaseEvent>
	{
		size_t operator()(const se::input::JoystickButtonReleaseEvent& event) const
		{
			se_assert(false && "TODO joystick pointer");
			return size_t(event.button);
		}
	};
	template<> struct hash<se::input::JoystickAxisEvent>
	{
		size_t operator()(const se::input::JoystickAxisEvent& event) const
		{
			se_assert(false && "TODO joystick pointer");
			uint32_t data[2];
			data[0] = event.axisIndex;
			data[1] = uint32_t(event.axisMovement);
			return size_t(se::murmurHash3_x86_32(data, 8, 0));
		}
	};
	template<> struct hash<se::input::QuitEvent>
	{
		size_t operator()(const se::input::QuitEvent& event) const
		{
			return size_t(1);
		}
	};
	template<> struct hash<se::input::FileDropEvent>
	{
		size_t operator()(const se::input::FileDropEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(event.filepath.data(), event.filepath.size(), 0));
		}
	};
}
