#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/PropertyPrecedence.h"


namespace se::gui
{
	class SE_INTERFACE IPropertyLink
	{
	public:
		virtual ~IPropertyLink() = default;
		virtual const void* getValue(const IPropertyHost&) const = 0;
		virtual void setValue(IPropertyHost&, const void*, PropertyPrecedence) const = 0;
		virtual void setTriggerValue(IPropertyHost&, const std::any&, PropertyPrecedence) const = 0;
		virtual bool compareValue(const IPropertyHost&, const std::any&) const = 0;
		virtual void resetValue(IPropertyHost&, PropertyPrecedence) const = 0;
		virtual const std::type_info& getType() const = 0;
		virtual const std::string& getName() const = 0;
	};

	class SE_INTERFACE IPropertyLinkIterator
	{
	public:
		virtual ~IPropertyLinkIterator() = default;

	};
}
