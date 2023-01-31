#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/PropertyValueType.h"


namespace se::gui
{
	class IPropertyLink
	{
	public:
		virtual ~IPropertyLink() = default;
		virtual const void* getValue(const IPropertyHost&) const = 0;
		virtual void setValue(IPropertyHost&, const void*, PropertyValueType) const = 0;
		virtual void setTriggerValue(IPropertyHost&, const std::any&, PropertyValueType) const = 0;
		virtual bool compareValue(const IPropertyHost&, const std::any&) const = 0;
		virtual void resetValue(IPropertyHost&, PropertyValueType) const = 0;
		virtual const std::type_info& getType() const = 0;
		virtual const std::string& getName() const = 0;
	};
}
