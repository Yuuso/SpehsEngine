#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventGenerator.h"

#include "SpehsEngine/Input/EventSignaler.h"


namespace se
{
	namespace input
	{
		CustomEventGenerator::CustomEventGenerator(EventSignaler& _eventSignaler)
			: eventSignaler(_eventSignaler)
		{
			unregisterCustomEventFunction.reset(new std::function<void(const uint32_t)>());
			*unregisterCustomEventFunction =
				[this](const uint32_t registeredCustomEventId)
				{
					const std::unordered_map<uint32_t, RegisteredCustomEvent>::iterator it = registeredCustomEvents.find(registeredCustomEventId);
					se_assert(it != registeredCustomEvents.end());
					if (it != registeredCustomEvents.end())
					{
						registeredCustomEvents.erase(it);
					}
				};
		}

		CustomEventGenerator::~CustomEventGenerator()
		{
			se_assert(unregisterCustomEventFunction);
			if (unregisterCustomEventFunction)
			{
				*unregisterCustomEventFunction = std::function<void(const uint32_t)>();
			}
		}
	}
}
