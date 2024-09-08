#pragma once


namespace se
{
	namespace input
	{
		enum KeyboardKey : unsigned int;
		enum class Key : uint32_t;
		enum class MouseButton : uint32_t;
		enum class EventType : uint8_t;
		enum class JoystickHatState : uint8_t;
		struct FileDropEvent;
		struct JoystickAxisEvent;
		struct JoystickButtonEvent;
		struct JoystickHatEvent;
		struct JoystickId;
		struct JoystickGuid;
		struct KeyboardEvent;
		struct MouseButtonEvent;
		struct MouseHoverEvent;
		struct MouseMotionEvent;
		struct MouseWheelEvent;
		struct QuitEvent;
		struct TextInputEvent;
		struct CustomEventParameters;
		class EventCatcher;
		class EventSignaler;
		class CustomEventGenerator;
		class InputManager;

		const char* toString(const Key key);
		const char* toShortString(const Key key);
	}
}

#include "SpehsEngine/Input/Event.h"
#include "SpehsEngine/Input/EventUtilityFunctions.h"
#include "SpehsEngine/Input/Key.h"
#include "SpehsEngine/Input/MouseButton.h"
#include "SpehsEngine/Input/JoystickHatState.h"
#include "SpehsEngine/Input/CustomEventParameters.h"
#include "SpehsEngine/Input/CustomEventParametersUtilityFunctions.h"
