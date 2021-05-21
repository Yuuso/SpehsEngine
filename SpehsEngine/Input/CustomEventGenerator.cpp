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

		}
	}
}
