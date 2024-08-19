#pragma once


namespace se
{
	namespace input
	{
		enum class EventType : uint8_t;
		struct FileDropEvent;
		struct JoystickAxisEvent;
		struct JoystickButtonEvent;
		struct JoystickHatEvent;
		struct KeyboardEvent;
		struct MouseButtonEvent;
		struct MouseHoverEvent;
		struct MouseMotionEvent;
		struct MouseWheelEvent;
		struct QuitEvent;
		struct TextInputEvent;
		class EventCatcher;
		class EventSignaler;
	}
}
