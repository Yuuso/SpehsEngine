#pragma once

#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Core/Serial/Serial.h"
#include "SpehsEngine/Input/CustomEventParameters.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"


template<> template<typename S, typename T>
static bool se::Serial<se::input::KeyboardEventParameters>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.key, "key");
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::MouseButtonEventParameters>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.mouseButton, "mouseButton");
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::MouseMotionEventParameters>::serial(S& _serial, T _value)
{
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::MouseWheelEventParameters>::serial(S& _serial, T _value)
{
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::MouseHoverEventParameters>::serial(S& _serial, T _value)
{
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::JoystickButtonEventParameters>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.joystickGuid, "joystickGuid");
	se_serial(_serial, _value.buttonIndex, "buttonIndex");
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::JoystickAxisEventParameters>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.joystickGuid, "joystickGuid");
	se_serial(_serial, _value.axisIndex, "axisIndex");
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::JoystickHatEventParameters>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.joystickGuid, "joystickGuid");
	se_serial(_serial, _value.hatIndex, "hatIndex");
	return true;
}

template<> template<typename S, typename T>
static bool se::Serial<se::input::CustomEventParameters>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.eventType, "eventType");
	switch (_value.eventType)
	{
	case input::EventType::none: break;
	case input::EventType::quit: break;
	case input::EventType::textInput: break;
	case input::EventType::fileDrop: break;
	case input::EventType::keyboard:		se_serial(_serial, _value.keyboardEventParameters, "keyboardEventParameters"); break;
	case input::EventType::mouseButton:		se_serial(_serial, _value.mouseButtonEventParameters, "mouseButtonEventParameters"); break;
	case input::EventType::mouseMotion:		se_serial(_serial, _value.mouseMotionEventParameters, "mouseMotionEventParameters"); break;
	case input::EventType::mouseWheel:		se_serial(_serial, _value.mouseWheelEventParameters, "mouseWheelEventParameters"); break;
	case input::EventType::mouseHover:		se_serial(_serial, _value.mouseHoverEventParameters, "mouseHoverEventParameters"); break;
	case input::EventType::joystickButton:	se_serial(_serial, _value.joystickButtonEventParameters, "joystickButtonEventParameters"); break;
	case input::EventType::joystickAxis:	se_serial(_serial, _value.joystickAxisEventParameters, "joystickAxisEventParameters"); break;
	case input::EventType::joystickHat:		se_serial(_serial, _value.joystickHatEventParameters, "joystickHatEventParameters"); break;
	}
	return true;
}

namespace std
{
	template<> struct hash<se::input::KeyboardEventParameters>
	{
		size_t operator()(const se::input::KeyboardEventParameters& eventParameters) const
		{
			return size_t(eventParameters.key);
		}
	};
	template<> struct hash<se::input::MouseButtonEventParameters>
	{
		size_t operator()(const se::input::MouseButtonEventParameters& eventParameters) const
		{
			return size_t(eventParameters.mouseButton);
		}
	};
	template<> struct hash<se::input::MouseMotionEventParameters>
	{
		size_t operator()(const se::input::MouseMotionEventParameters& eventParameters) const
		{
			return size_t(1);
		}
	};
	template<> struct hash<se::input::MouseWheelEventParameters>
	{
		size_t operator()(const se::input::MouseWheelEventParameters& eventParameters) const
		{
			return size_t(1);
		}
	};
	template<> struct hash<se::input::MouseHoverEventParameters>
	{
		size_t operator()(const se::input::MouseHoverEventParameters& eventParameters) const
		{
			return size_t(1);
		}
	};
	template<> struct hash<se::input::JoystickButtonEventParameters>
	{
		size_t operator()(const se::input::JoystickButtonEventParameters& eventParameters) const
		{
			return size_t(se::Murmur3::impl((const char*)&eventParameters.joystickGuid.data, 17, 0));
		}
	};
	template<> struct hash<se::input::JoystickAxisEventParameters>
	{
		size_t operator()(const se::input::JoystickAxisEventParameters& eventParameters) const
		{
			return size_t(se::Murmur3::impl((const char*)&eventParameters.joystickGuid.data, 17, 0));
		}
	};
	template<> struct hash<se::input::JoystickHatEventParameters>
	{
		size_t operator()(const se::input::JoystickHatEventParameters& eventParameters) const
		{
			return size_t(se::Murmur3::impl((const char*)&eventParameters.joystickGuid.data, 17, 0));
		}
	};

	template<> struct hash<se::input::CustomEventParameters>
	{
		size_t operator()(const se::input::CustomEventParameters& customEventParameters) const
		{
			size_t data[2];
			data[0] = size_t(customEventParameters.eventType);
			switch (customEventParameters.eventType)
			{
			case se::input::EventType::none: break;
			case se::input::EventType::fileDrop: break;
			case se::input::EventType::quit: break;
			case se::input::EventType::textInput: break;
			case se::input::EventType::keyboard: data[1] = std::hash<se::input::KeyboardEventParameters>()(customEventParameters.keyboardEventParameters); break;
			case se::input::EventType::mouseButton: data[1] = std::hash<se::input::MouseButtonEventParameters>()(customEventParameters.mouseButtonEventParameters); break;
			case se::input::EventType::mouseMotion: data[1] = std::hash<se::input::MouseMotionEventParameters>()(customEventParameters.mouseMotionEventParameters); break;
			case se::input::EventType::mouseWheel: data[1] = std::hash<se::input::MouseWheelEventParameters>()(customEventParameters.mouseWheelEventParameters); break;
			case se::input::EventType::mouseHover: data[1] = std::hash<se::input::MouseHoverEventParameters>()(customEventParameters.mouseHoverEventParameters); break;
			case se::input::EventType::joystickButton: data[1] = std::hash<se::input::JoystickButtonEventParameters>()(customEventParameters.joystickButtonEventParameters); break;
			case se::input::EventType::joystickAxis: data[1] = std::hash<se::input::JoystickAxisEventParameters>()(customEventParameters.joystickAxisEventParameters); break;
			case se::input::EventType::joystickHat: data[1] = std::hash<se::input::JoystickHatEventParameters>()(customEventParameters.joystickHatEventParameters); break;
			}
			return size_t(se::Murmur3::impl((const char*)data, sizeof(size_t) * 2, 0));
		}
	};
}
