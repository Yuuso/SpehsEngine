#pragma once


namespace se
{
	namespace input
	{
		class ScopedCustomEvent
		{
		public:
			ScopedCustomEvent() noexcept = default;
			ScopedCustomEvent(ScopedCustomEvent&& move) noexcept
			{
				swap(move);
			}
			ScopedCustomEvent(const ScopedCustomEvent& copy) = delete;
			~ScopedCustomEvent()
			{
				if (registeredCustomEventId && unregisterCustomEventFunction && *unregisterCustomEventFunction)
				{
					(*unregisterCustomEventFunction)(*registeredCustomEventId);
				}
			}
			ScopedCustomEvent& operator=(ScopedCustomEvent&& move) noexcept
			{
				swap(move);
			}
			ScopedCustomEvent& operator=(const ScopedCustomEvent& copy) = delete;

			void swap(ScopedCustomEvent& other)
			{
				std::swap(registeredCustomEventId, other.registeredCustomEventId);
				std::swap(unregisterCustomEventFunction, other.unregisterCustomEventFunction);
			}

			explicit operator bool() const { return bool(registeredCustomEventId); }

		private:
			friend class CustomEventGenerator;
			std::shared_ptr<uint32_t> registeredCustomEventId;
			std::shared_ptr<std::function<void(const uint32_t)>> unregisterCustomEventFunction;
		};
	}
}
