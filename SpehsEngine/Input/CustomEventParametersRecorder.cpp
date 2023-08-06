#include "stdafx.h"
#include "SpehsEngine/Input/CustomEventParametersRecorder.h"

#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/JoystickUtilityFunctions.h"


namespace se
{
	namespace input
	{
		CustomEventParametersRecorder::CustomEventParametersRecorder(EventSignaler& _eventSignaler)
			: eventSignaler(_eventSignaler)
		{
			eventSignaler.connectToKeyboardSignal(scopedConnections.add(),
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
			eventSignaler.connectToMouseButtonSignal(scopedConnections.add(),
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
			eventSignaler.connectToJoystickButtonSignal(scopedConnections.add(),
				[this](const JoystickButtonEvent& event)
				{
					if (event.isPress())
					{
						customEventParameters.emplace();
						customEventParameters->eventType = EventType::joystickButton;
						customEventParameters->joystickButtonEventParameters.joystickGuid = getJoystickGuid(event.joystickId);
						customEventParameters->joystickButtonEventParameters.buttonIndex = event.buttonIndex;
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
