#pragma once

#include "stdafx.h" //
#include "SpehsEngine/GUI/Binding.h"
#include "SpehsEngine/GUI/PropertySetter.h"


namespace se::gui
{
	class Element;
	class IPropertyHost;


	class ITriggerAction
	{
	public:
		virtual void execute(Element*) const = 0;
	};

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


	class PropertyTrigger
	{
	public:
		PropertyTrigger(std::string_view _propertyName, const std::any& _value,
				std::initializer_list<PropertySetter> _setters,
				std::initializer_list<std::shared_ptr<const ITriggerAction>> _enterActions = {},
				std::initializer_list<std::shared_ptr<const ITriggerAction>> _exitActions = {})
			: binding(_propertyName, RelativeSourceMode::Self)
			, value(_value)
			, setters(_setters)
			, enterActions(_enterActions)
			, exitActions(_exitActions) {}
		PropertyTrigger(const Binding& _binding, const std::any& _value,
				std::initializer_list<PropertySetter> _setters,
				std::initializer_list<std::shared_ptr<const ITriggerAction>> _enterActions = {},
				std::initializer_list<std::shared_ptr<const ITriggerAction>> _exitActions = {})
			: binding(_binding)
			, value(_value)
			, setters(_setters)
			, enterActions(_enterActions)
			, exitActions(_exitActions) {}

		const Binding binding;
		const std::any value;
		std::vector<PropertySetter> setters;
		std::vector<std::shared_ptr<const ITriggerAction>> enterActions;
		std::vector<std::shared_ptr<const ITriggerAction>> exitActions;
	};

	class PropertyTriggerLink
	{
	public:
		PropertyTriggerLink(const PropertyTrigger& _trigger, PropertyValueType _type)
			: trigger(_trigger), type(_type) {}
		const PropertyTrigger trigger;
		const PropertyValueType type;
		IPropertyHost* source = nullptr;
		ScopedConnection sourceChangedConnection;
		bool active = false;
	};
}
