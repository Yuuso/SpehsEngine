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
			eventSignaler.connectToKeyboardSignal(addScopedConnection(),
				[this](const KeyboardEvent& event)
				{
					if (event.type == KeyboardEvent::Type::Press)
					{
						customEventParameters.emplace();
						customEventParameters->eventType = EventType::keyboard;
						customEventParameters->keyboardEventParameters.key = event.key;
						scopedConnections.clear();
						return true;
					}
					else
					{
						return false;
					}
				}, EventSignaler::maxPriority);
			eventSignaler.connectToMouseButtonSignal(addScopedConnection(),
				[this](const MouseButtonEvent& event)
				{
					if (event.isPress())
					{
						customEventParameters.emplace();
						customEventParameters->eventType = EventType::mouseButton;
						customEventParameters->mouseButtonEventParameters.mouseButton = event.button;
						scopedConnections.clear();
						return true;
					}
					else
					{
						return false;
					}
				}, EventSignaler::maxPriority);
		}
	}
}
