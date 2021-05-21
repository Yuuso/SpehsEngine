#pragma once

#include "SpehsEngine/Input/CustomEventParameters.h"


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
			std::vector<boost::signals2::scoped_connection> scopedConnections;
		};
	}
}
