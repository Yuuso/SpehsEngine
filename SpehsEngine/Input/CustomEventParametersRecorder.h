#pragma once

#include "SpehsEngine/Core/ScopedConnections.h"
#include "SpehsEngine/Input/CustomEventParameters.h"


namespace se
{
	namespace input
	{
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
