#pragma once

#include "SpehsEngine/Input/CustomEventParameters.h"


namespace se
{
	namespace input
	{
		class EventSignaler;

		class ScopedCustomEvent
		{
		public:
			ScopedCustomEvent() = default;
			ScopedCustomEvent(const ScopedCustomEvent&& move)
				: registeredCustomEventId(move.registeredCustomEventId)
			{

			}
			ScopedCustomEvent(const ScopedCustomEvent& copy)
				: registeredCustomEventId(copy.registeredCustomEventId)
			{

			}
			ScopedCustomEvent& operator=(const ScopedCustomEvent&& move)
			{
				registeredCustomEventId = move.registeredCustomEventId;
			}
			ScopedCustomEvent& operator=(const ScopedCustomEvent& copy)
			{
				registeredCustomEventId = copy.registeredCustomEventId;
			}

			operator bool() const { return bool(registeredCustomEventId); }

		private:
			friend class CustomEventGenerator;
			std::shared_ptr<uint32_t> registeredCustomEventId;
		};

		// Turns a spehs engine input event into a custom event
		class CustomEventGenerator
		{
		public:

			CustomEventGenerator(EventSignaler& _eventSignaler);

			template<typename CustomEvent>
			ScopedCustomEvent addCustomEvent(const CustomEventParameters& customEventParameters, const CustomEvent generatedCustomEvent, const int priority)
			{
				static_assert(std::is_copy_assignable<CustomEvent>::value);
				boost::signals2::scoped_connection& scopedConnection = registeredCustomEvents.back().scopedConnection;
#define SE_INPUT_EVENT_CASE(p_EventName, p_EventType) \
				case EventType::p_EventType: \
					const uint32_t registeredCustomEventId = nextRegisteredCustomEventId++; \
					RegisteredCustomEvent& registeredCustomEvent = registeredCustomEvents[registeredCustomEventId] = RegisteredCustomEvent(); \
					registeredCustomEvent.id.reset(new uint32_t(registeredCustomEventId)); \
					ScopedCustomEvent scopedCustomEvent; \
					scopedCustomEvent.registeredCustomEventId = registeredCustomEvent.id; \
					eventSignaler.connectTo##p_EventName##Signal(registeredCustomEvent.scopedConnection, [this, customEventParameters, generatedCustomEvent, registeredCustomEventId](const p_EventName##Event& event) \
						{ \
							const std::unordered_map<uint32_t, RegisteredCustomEvent>::iterator it = registeredCustomEvents.find(registeredCustomEventId); \
							se_assert(it != registeredCustomEvents.end()); \
							if (it != registeredCustomEvents.end()) \
							{ \
								if (it->second.id.use_count() == 1) \
								{ \
									registeredCustomEvents.erase(it); \
									return false; \
								} \
								else if (event == customEventParameters.p_EventType##Event) \
								{ \
									eventSignaler.signalCustomEvent(generatedCustomEvent); \
									return true; \
								} \
								else \
								{ \
									return false; \
								} \
							} \
						}, priority); \
					return scopedCustomEvent;
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
					break;
				}
#undef SE_INPUT_EVENT_CASE
				return ScopedCustomEvent();
			}

		private:

			struct RegisteredCustomEvent
			{
				std::shared_ptr<uint32_t> id;
				boost::signals2::scoped_connection scopedConnection;
			};

			EventSignaler& eventSignaler;
			uint32_t nextRegisteredCustomEventId = 1;
			std::unordered_map<uint32_t, RegisteredCustomEvent> registeredCustomEvents;
		};
	}
}
