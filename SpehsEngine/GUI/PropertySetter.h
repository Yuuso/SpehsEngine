#pragma once

#include "stdafx.h"


namespace se::gui
{
	class IPropertySetter
	{
	public:
		virtual ~IPropertySetter() = default;
		virtual const std::string& getPropertyName() const = 0;
		virtual const std::any& getPropertyValue() const = 0;
	};

	class PropertySetter : public IPropertySetter
	{
	public:
		PropertySetter(std::string_view _name, const std::any&& _value)
			: propertyName(_name), value(_value) {}
		const std::string& getPropertyName() const override
		{
			return propertyName;
		}
		const std::any& getPropertyValue() const override
		{
			return value;
		}
		std::string propertyName;
		std::any value;
	};

	class EmptyPropertySetter : public IPropertySetter
	{
	public:
		EmptyPropertySetter(const PropertySetter& _ref)
			: ref(_ref) {}
		const std::string& getPropertyName() const override
		{
			return ref.getPropertyName();
		}
		const std::any& getPropertyValue() const override
		{
			static const/*expr*/ std::any empty{};
			return empty;
		}
	private:
		const PropertySetter& ref;
	};
}
