#pragma once

#include "SpehsEngine/Core/ScopedConnections.h"
#include "SpehsEngine/Input/CustomEventParameters.h"
#include <optional>


namespace se
{
	namespace input
	{
		class EventSignaler;

		// Helper class for recording user inputs to be used to generate custom events.
		class CustomEventParametersRecorder
		{
		public:

			CustomEventParametersRecorder(EventSignaler& _eventSignaler);

			const std::optional<CustomEventParameters>& getCustomEventParameters() const { return customEventParameters; }

		private:
			EventSignaler& eventSignaler;
			std::optional<CustomEventParameters> customEventParameters;
			ScopedConnections scopedConnections;
		};
	}
}
