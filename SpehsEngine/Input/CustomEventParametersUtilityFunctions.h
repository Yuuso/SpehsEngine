#pragma once

#include "SpehsEngine/Core/Archive.h"
#include "SpehsEngine/Core/Murmur3.h"
#include "SpehsEngine/Input/CustomEventParameters.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"


namespace se
{
	Archive writeToArchive(const input::KeyboardEventParameters& eventParameters);
	Archive writeToArchive(const input::MouseButtonEventParameters& eventParameters);
	Archive writeToArchive(const input::MouseMotionEventParameters& eventParameters);
	Archive writeToArchive(const input::MouseWheelEventParameters& eventParameters);
	Archive writeToArchive(const input::MouseHoverEventParameters& eventParameters);
	Archive writeToArchive(const input::JoystickButtonEventParameters& eventParameters);
	Archive writeToArchive(const input::JoystickAxisEventParameters& eventParameters);

	bool readFromArchive(const Archive& archive, input::KeyboardEventParameters& eventParameters);
	bool readFromArchive(const Archive& archive, input::MouseButtonEventParameters& eventParameters);
	bool readFromArchive(const Archive& archive, input::MouseMotionEventParameters& eventParameters);
	bool readFromArchive(const Archive& archive, input::MouseWheelEventParameters& eventParameters);
	bool readFromArchive(const Archive& archive, input::MouseHoverEventParameters& eventParameters);
	bool readFromArchive(const Archive& archive, input::JoystickButtonEventParameters& eventParameters);
	bool readFromArchive(const Archive& archive, input::JoystickAxisEventParameters& eventParameters);

	Archive writeToArchive(const input::CustomEventParameters& customEventParameters);
	bool readFromArchive(const Archive& archive, input::CustomEventParameters& customEventParameters);
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
			return size_t(eventParameters.button);
		}
	};
	template<> struct hash<se::input::JoystickAxisEventParameters>
	{
		size_t operator()(const se::input::JoystickAxisEventParameters& eventParameters) const
		{
			return size_t(eventParameters.axisIndex);
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
			}
			return size_t(se::murmurHash3_x86_32(data, sizeof(size_t) * 2, 0));
		}
	};
}
