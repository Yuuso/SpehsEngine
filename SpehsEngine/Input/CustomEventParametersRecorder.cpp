#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParametersRecorder.h"

#include "SpehsEngine/Input/EventSignaler.h"


namespace se
{
	namespace input
	{
		CustomEventParametersRecorder::CustomEventParametersRecorder(EventSignaler& _eventSignaler)
			: eventSignaler(_eventSignaler)
		{
			auto addScopedConnection = [this]()->boost::signals2::scoped_connection&
			{
				scopedConnections.push_back(boost::signals2::scoped_connection());
				return scopedConnections.back();
			};
			eventSignaler.connectToKeyboardPressSignal(addScopedConnection(),
				[this](const KeyboardPressEvent& event)
				{
					customEventParameters.emplace();
					customEventParameters->eventType = EventType::keyboardPress;
					customEventParameters->keyboardPressEvent = event;
					scopedConnections.clear();
					return true;
				}, EventSignaler::maxPriority);
			eventSignaler.connectToMouseButtonPressSignal(addScopedConnection(),
				[this](const MouseButtonPressEvent& event)
				{
					customEventParameters.emplace();
					customEventParameters->eventType = EventType::mouseButtonPress;
					customEventParameters->mouseButtonPressEvent = event;
					scopedConnections.clear();
					return true;
				}, EventSignaler::maxPriority);
		}
	}
}
