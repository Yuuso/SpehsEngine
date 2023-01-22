#pragma once

#include "stdafx.h" //
#include "SpehsEngine/GUI/Binding.h"


namespace se::gui
{
	class TriggerBase
	{
	public:
	};


	struct EventArgs;
	class Element;

	class TriggerAction
	{
	public:
		virtual void execute(Element*) const = 0;
	};

	class EventTrigger
	{
	public:
		EventTrigger(std::string_view _eventName,
					 std::initializer_list<std::shared_ptr<const TriggerAction>> _actions)
			: eventName(_eventName)
			, actions(_actions) {}
		const std::string eventName;
		const std::vector<std::shared_ptr<const TriggerAction>> actions;
	};


	class Setter
	{
	public:
		const std::string propertyName;
		const std::any value;
	};

	class Trigger
	{
	public:
		Trigger(std::string_view _propertyName, const std::any& _value,
				std::initializer_list<Setter> _setters,
				std::initializer_list<std::shared_ptr<const TriggerAction>> _enterActions = {},
				std::initializer_list<std::shared_ptr<const TriggerAction>> _exitActions = {})
			: binding(_propertyName, RelativeSourceMode::Self)
			, value(_value)
			, setters(_setters)
			, enterActions(_enterActions)
			, exitActions(_exitActions) {}
		Trigger(const Binding& _binding, const std::any& _value,
				std::initializer_list<Setter> _setters,
				std::initializer_list<std::shared_ptr<const TriggerAction>> _enterActions = {},
				std::initializer_list<std::shared_ptr<const TriggerAction>> _exitActions = {})
			: binding(_binding)
			, value(_value)
			, setters(_setters)
			, enterActions(_enterActions)
			, exitActions(_exitActions) {}

		const Binding binding;
		const std::any value;
		const std::vector<Setter> setters;
		const std::vector<std::shared_ptr<const TriggerAction>> enterActions;
		const std::vector<std::shared_ptr<const TriggerAction>> exitActions;
	};

	class TriggerLink
	{
	public:
		TriggerLink(const Trigger& _trigger)
			: trigger(_trigger) {}
		const Trigger trigger;
		ScopedConnection sourceChangedConnection;
		bool active = false;
	};
}
