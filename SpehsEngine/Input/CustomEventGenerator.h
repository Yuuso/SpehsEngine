#pragma once

#include "SpehsEngine/Input/CustomEventParameters.h"


namespace se
{
	namespace input
	{
		class EventSignaler;

		// Turns a spehs engine input event into a custom event
		class CustomEventGenerator
		{
		public:

			CustomEventGenerator(EventSignaler& _eventSignaler);

			template<typename CustomEvent>
			void addCustomEvent(const CustomEventParameters& customEventParameters, const CustomEvent generatedCustomEvent, const int priority)
			{
				static_assert(std::is_copy_assignable<CustomEvent>::value);
				boost::signals2::scoped_connection& scopedConnection = registeredCustomEvents.back().scopedConnection;
#define SE_INPUT_EVENT_CASE(p_EventName, p_EventType) \
				case EventType::p_EventType: \
					registeredCustomEvents.push_back(RegisteredCustomEvent()); \
					eventSignaler.connectTo##p_EventName##Signal(registeredCustomEvents.back().scopedConnection, [this, customEventParameters, generatedCustomEvent](const p_EventName##Event& event) \
						{ \
							if (event == customEventParameters.p_EventType##Event) \
							{ \
								eventSignaler.signalCustomEvent(generatedCustomEvent); \
								return true; \
							} \
							else \
							{ \
								return false; \
							} \
						}, priority); \
					break;
				switch (customEventParameters.eventType)
				{
					SE_INPUT_EVENT_CASE(KeyboardPress, keyboardPress)
					SE_INPUT_EVENT_CASE(KeyboardDown, keyboardDown)
					SE_INPUT_EVENT_CASE(KeyboardRelease, keyboardRelease)
					SE_INPUT_EVENT_CASE(MouseButtonPress, mouseButtonPress)
					SE_INPUT_EVENT_CASE(MouseButtonDown, mouseButtonDown)
					SE_INPUT_EVENT_CASE(MouseButtonRelease, mouseButtonRelease)
					SE_INPUT_EVENT_CASE(MouseMotion, mouseMotion)
					SE_INPUT_EVENT_CASE(MouseWheel, mouseWheel)
					SE_INPUT_EVENT_CASE(MouseHover, mouseHover)
					//SE_INPUT_EVENT_CASE(JoystickButtonPress, joystickButtonPress)
					//SE_INPUT_EVENT_CASE(JoystickButtonDown, joystickButtonDown)
					//SE_INPUT_EVENT_CASE(JoystickButtonRelease, joystickButtonRelease)
					//SE_INPUT_EVENT_CASE(JoystickAxis, joystickAxis)
					SE_INPUT_EVENT_CASE(Quit, quit)
					SE_INPUT_EVENT_CASE(TextInput, textInput)
					SE_INPUT_EVENT_CASE(FileDrop, fileDrop)
				case EventType::joystickButtonPress:
				case EventType::joystickButtonDown:
				case EventType::joystickButtonRelease:
				case EventType::joystickAxis:
				case EventType::none:
					log::warning("CustomEventGenerator::addEvent() EventType not supported: " + std::to_string(int(customEventParameters.eventType)));
					return;
				}
#undef SE_INPUT_EVENT_CASE
			}

		private:

			struct RegisteredCustomEvent
			{
				boost::signals2::scoped_connection scopedConnection;
			};

			EventSignaler& eventSignaler;
			std::vector<RegisteredCustomEvent> registeredCustomEvents;
		};
	}
}
