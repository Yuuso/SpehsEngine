#pragma once

#include "SpehsEngine/GUI/TriggerAction.h"


namespace se::gui
{
	class LogOutputAction : public ITriggerAction
	{
	public:
		LogOutputAction() = delete;
		LogOutputAction(std::string_view _message)
			: message(_message) {}
		void execute(Element* _source) const override
		{
			const std::string name =
				(_source && !_source->getName().empty())
				? _source->getName() + ": "
				: "";
			log::info("LogOutputAction: " + name + message);
		}
	private:
		const std::string message;
	};
	//class SetPropertyAction : public ITriggerAction
	//{
	//public:
	//	SetPropertyAction() = delete;
	//	SetPropertyAction(std::string_view _propertyName, const std::any& _value)
	//		: propertyName(_propertyName), value(_value) {}
	//	void execute(Element* _source) const override
	//	{
	//		if (_source)
	//		{
	//			_source->setProperty(propertyName, value);
	//		}
	//	}
	//private:
	//	const std::string propertyName;
	//	const std::any value;
	//};
	class BeginStoryboardAction : public ITriggerAction
	{};
	class InvokeCommandAction : public ITriggerAction
	{};
	class RemoveElementAction : public ITriggerAction
	{};
}
