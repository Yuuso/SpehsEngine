#pragma once

#include "stdafx.h" //


namespace se::gui
{
	class IPropertyLink;

	class SE_INTERFACE IPropertyHost
	{
	public:
		virtual ~IPropertyHost() = default;
		virtual const IPropertyLink* getPropertyLink(const std::string&) = 0;
		virtual std::vector<const IPropertyLink*> getPropertyLinks() = 0;

		Connection onPropertyChanged(const std::function<void(const std::string&, const void*)>& func)
		{
			return propertyChangedSignal.connect(func);
		}
		void propertyChanged(const std::string& _name, const void* _value)
		{
			propertyChangedSignal(_name, _value);
		}

	private:
		Signal<void(const std::string&, const void*)> propertyChangedSignal;
	};
}