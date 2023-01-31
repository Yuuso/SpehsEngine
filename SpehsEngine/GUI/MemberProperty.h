#pragma once

#include "SpehsEngine/GUI/PropertyLink.h"


namespace se::gui
{
#define GUI_REGISTER_MEMBER_PROPERTY(NAME) \
	_meta.registerMemberProperty(#NAME, SelfClass::property_##NAME##Default, &SelfClass::property_##NAME);


	template<typename PropertyType>
	class MemberProperty
	{
	public:
		MemberProperty(const PropertyType& _default)
			: value(_default) {}

		const PropertyType& getValue() const
		{
			if (triggerValue)
				return *triggerValue;
			return value;
		}
		bool setValue(const PropertyType& _value, PropertyValueType _type)
		{
			if (type > _type)
				return false;
			if (value == _value)
				return false;

			value = _value;
			type = _type;
			if (triggerType < _type)
			{
				// override
				triggerValue = nullptr;
			}
			return triggerValue == nullptr;
		}
		bool setTriggerValue(const std::any& _value, PropertyValueType _type)
		{
			if (type > _type || triggerType > _type)
				return false;

			triggerType = _type;
			if (!_value.has_value())
			{
				se_assert(triggerValue != nullptr);
				triggerValue = nullptr;
				triggerType = PropertyValueType::Default;
			}
			else
			{
				triggerValue = &std::any_cast<const PropertyType&>(_value);
			}
			return true;
		}

	private:
		PropertyValueType type = PropertyValueType::Default;
		PropertyValueType triggerType = PropertyValueType::Default;
		PropertyType value;
		const PropertyType* triggerValue = nullptr;
	};


	template<typename PropertyType, typename HostType>
	using MemberPropertyPtr = MemberProperty<PropertyType> HostType::*;

	template<typename PropertyType, typename HostType>
	class MemberPropertyLink : public IPropertyLink
	{
		static_assert(std::is_base_of<IPropertyHost, HostType>::value,
			"MemberPropertyLink HostType must inherit IPropertyHost");
	public:
		MemberPropertyLink(
				const std::string& _name,
				const PropertyType& _defaultValue,
				const MemberPropertyPtr<PropertyType, HostType>& _ptr)
			: name(_name), defaultValue(_defaultValue), propertyPtr(_ptr) {}

		const void* getValue(const IPropertyHost& _host) const override
		{
			return static_cast<const void*>(&getProperty(_host).getValue());
		}
		void setValue(IPropertyHost& _host, const void* _value, PropertyValueType _type) const override
		{
			MemberProperty<PropertyType>& prop = getProperty(_host);
			if (prop.setValue(*static_cast<const PropertyType*>(_value), _type))
			{
				_host.propertyChanged(name, static_cast<const void*>(&prop.getValue()));
			}
		}
		void setTriggerValue(IPropertyHost& _host, const std::any& _value, PropertyValueType _type) const override
		{
			MemberProperty<PropertyType>& prop = getProperty(_host);
			if (prop.setTriggerValue(_value, _type))
			{
				_host.propertyChanged(name, static_cast<const void*>(&prop.getValue()));
			}
		}
		bool compareValue(const IPropertyHost& _host, const std::any& _value) const override
		{
			return getProperty(_host).getValue() == std::any_cast<const PropertyType&>(_value);
		}
		void resetValue(IPropertyHost& _host, PropertyValueType _type) const override
		{
			setValue(_host, static_cast<const void*>(&defaultValue), _type);
		}
		const std::type_info& getType() const override
		{
			return typeid(PropertyType);
		}
		const std::string& getName() const override
		{
			return name;
		}

	private:
		const MemberProperty<PropertyType>& getProperty(const IPropertyHost& _host) const
		{
			return static_cast<const HostType&>(_host).*propertyPtr;
		}
		MemberProperty<PropertyType>& getProperty(IPropertyHost& _host) const
		{
			return static_cast<HostType&>(_host).*propertyPtr;
		}

		const std::string name;
		const PropertyType defaultValue;
		const MemberPropertyPtr<PropertyType, HostType> propertyPtr;
	};
}
