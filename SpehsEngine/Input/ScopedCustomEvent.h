#pragma once

#include <memory>


namespace se
{
	namespace input
	{
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
	}
}
