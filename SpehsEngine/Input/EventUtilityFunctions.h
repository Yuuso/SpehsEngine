#pragma once

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/Murmur3.h"


namespace se
{
	Archive writeToArchive(const input::KeyboardEvent& event);
	Archive writeToArchive(const input::TextInputEvent& event);
	Archive writeToArchive(const input::MouseButtonEvent& event);
	Archive writeToArchive(const input::MouseMotionEvent& event);
	Archive writeToArchive(const input::MouseWheelEvent& event);
	Archive writeToArchive(const input::MouseHoverEvent& event);
	Archive writeToArchive(const input::JoystickButtonEvent& event);
	Archive writeToArchive(const input::JoystickAxisEvent& event);
	Archive writeToArchive(const input::QuitEvent& event);
	Archive writeToArchive(const input::FileDropEvent& event);

	bool readFromArchive(const Archive& archive, input::KeyboardEvent& event);
	bool readFromArchive(const Archive& archive, input::TextInputEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseButtonEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseMotionEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseWheelEvent& event);
	bool readFromArchive(const Archive& archive, input::MouseHoverEvent& event);
	bool readFromArchive(const Archive& archive, input::JoystickButtonEvent& event);
	bool readFromArchive(const Archive& archive, input::JoystickAxisEvent& event);
	bool readFromArchive(const Archive& archive, input::QuitEvent& event);
	bool readFromArchive(const Archive& archive, input::FileDropEvent& event);
}

namespace std
{
	template<> struct hash<se::input::KeyboardEvent>
	{
		size_t operator()(const se::input::KeyboardEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(&event, 9, 0));
		}
	};
	template<> struct hash<se::input::TextInputEvent>
	{
		size_t operator()(const se::input::TextInputEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(event.buffer.data(), event.buffer.size(), 0));
		}
	};
	template<> struct hash<se::input::MouseButtonEvent>
	{
		size_t operator()(const se::input::MouseButtonEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(&event, 5, 0));
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
	template<> struct hash<se::input::JoystickButtonEvent>
	{
		size_t operator()(const se::input::JoystickButtonEvent& event) const
		{
			return size_t(se::murmurHash3_x86_32(&event.button, 5, 0));
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
