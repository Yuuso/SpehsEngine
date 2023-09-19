#pragma once

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Core/Murmur3.h"


namespace std
{
	template<> struct hash<se::input::KeyboardEvent>
	{
		size_t operator()(const se::input::KeyboardEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event, 9, 0));
		}
	};
	template<> struct hash<se::input::TextInputEvent>
	{
		size_t operator()(const se::input::TextInputEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)event.buffer.data(), event.buffer.size(), 0));
		}
	};
	template<> struct hash<se::input::MouseButtonEvent>
	{
		size_t operator()(const se::input::MouseButtonEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event, 5, 0));
		}
	};
	template<> struct hash<se::input::MouseMotionEvent>
	{
		size_t operator()(const se::input::MouseMotionEvent& event) const
		{
			static_assert(sizeof(event) == 24);
			return size_t(se::Murmur3::impl((const char*)&event, sizeof(event), 0));
		}
	};
	template<> struct hash<se::input::MouseWheelEvent>
	{
		size_t operator()(const se::input::MouseWheelEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event.delta, 8, 0));
		}
	};
	template<> struct hash<se::input::MouseHoverEvent>
	{
		size_t operator()(const se::input::MouseHoverEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event.position, 8, 0));
		}
	};
	template<> struct hash<se::input::JoystickButtonEvent>
	{
		size_t operator()(const se::input::JoystickButtonEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event.joystickId, 22, 0));
		}
	};
	template<> struct hash<se::input::JoystickAxisEvent>
	{
		size_t operator()(const se::input::JoystickAxisEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event.joystickId, 25, 0));
		}
	};
	template<> struct hash<se::input::JoystickHatEvent>
	{
		size_t operator()(const se::input::JoystickHatEvent& event) const
		{
			return size_t(se::Murmur3::impl((const char*)&event.joystickId, 22, 0));
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
			return size_t(se::Murmur3::impl((const char*)event.filepath.data(), event.filepath.size(), 0));
		}
	};
}
