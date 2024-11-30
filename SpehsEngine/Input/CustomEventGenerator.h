#pragma once

#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/CustomEventParameters.h"
#include "SpehsEngine/Input/ScopedCustomEvent.h"


namespace se
{
	namespace input
	{
		// Turns a spehs engine input event into a custom event
		class CustomEventGenerator
		{
		public:

			CustomEventGenerator(EventSignaler& _eventSignaler);
			~CustomEventGenerator();

			template<typename CustomEvent>
			ScopedCustomEvent addCustomEvent(const CustomEventParameters& customEventParameters, const CustomEvent generatedCustomEvent, const int priority)
			{
				static_assert(std::is_base_of<ICustomEvent, CustomEvent>::value);
				static_assert(std::is_copy_assignable<CustomEvent>::value);
#define SE_INPUT_EVENT_CASE(p_EventName, p_EventType) \
				case EventType::p_EventType: \
				{ \
					const uint32_t registeredCustomEventId = nextRegisteredCustomEventId++; \
					RegisteredCustomEvent& registeredCustomEvent = registeredCustomEvents[registeredCustomEventId] = RegisteredCustomEvent(); \
					registeredCustomEvent.id.reset(new uint32_t(registeredCustomEventId)); \
					ScopedCustomEvent scopedCustomEvent; \
					scopedCustomEvent.registeredCustomEventId = registeredCustomEvent.id; \
					scopedCustomEvent.unregisterCustomEventFunction = unregisterCustomEventFunction; \
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
								else if (customEventParameters.p_EventType##EventParameters.check(event)) \
								{ \
									CustomEvent generatedCustomEvent2 = generatedCustomEvent; \
									customEventParameters.p_EventType##EventParameters.set(generatedCustomEvent2, event); \
									return eventSignaler.signalCustomEvent(generatedCustomEvent2); \
								} \
								else \
								{ \
									return false; \
								} \
							} \
							else \
							{ \
								return false; \
							} \
						}, priority); \
					return scopedCustomEvent; \
				}
				switch (customEventParameters.eventType)
				{
					SE_INPUT_EVENT_CASE(Keyboard, keyboard)
					SE_INPUT_EVENT_CASE(MouseButton, mouseButton)
					SE_INPUT_EVENT_CASE(MouseMotion, mouseMotion)
					SE_INPUT_EVENT_CASE(MouseWheel, mouseWheel)
					SE_INPUT_EVENT_CASE(MouseHover, mouseHover)
					SE_INPUT_EVENT_CASE(JoystickButton, joystickButton)
					SE_INPUT_EVENT_CASE(JoystickAxis, joystickAxis)
					SE_INPUT_EVENT_CASE(JoystickHat, joystickHat)
				case EventType::quit:
				case EventType::textInput:
				case EventType::fileDrop:
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
				ScopedConnection scopedConnection;
			};

			EventSignaler& eventSignaler;
			uint32_t nextRegisteredCustomEventId = 1;
			std::unordered_map<uint32_t, RegisteredCustomEvent> registeredCustomEvents;
			std::shared_ptr<std::function<void(const uint32_t)>> unregisterCustomEventFunction;
		};
	}
}
