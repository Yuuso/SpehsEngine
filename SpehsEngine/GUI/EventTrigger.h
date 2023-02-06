#pragma once

#include "stdafx.h" //
#include "SpehsEngine/GUI/TriggerAction.h"



namespace se::gui
{
	class EventTrigger
	{
	public:
		EventTrigger(std::string_view _eventName,
				std::initializer_list<std::shared_ptr<const ITriggerAction>> _actions)
			: eventName(_eventName)
			, actions(_actions) {}
		std::string eventName;
		std::vector<std::shared_ptr<const ITriggerAction>> actions;
	};
}
