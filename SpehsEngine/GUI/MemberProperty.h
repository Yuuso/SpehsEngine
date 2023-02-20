#pragma once

#include "boost/type_traits/has_equal_to.hpp"
#include "SpehsEngine/GUI/PropertyLink.h"


namespace se::gui
{
#define GUI_MEMBER_PROPERTY(NAME, TYPE, DEFAULT) \
private: \
	inline static const/*expr*/ TYPE property_##NAME##Default = DEFAULT; \
	::se::gui::MemberProperty<TYPE> property_##NAME{ property_##NAME##Default }; \
public: \
	SelfClass& set##NAME(const TYPE& _value) \
	{ \
		if (property_##NAME.setValue(_value, ::se::gui::PropertyPrecedence::Local)) \
		{ \
			propertyChanged(#NAME, \
				static_cast<const void*>(&property_##NAME.getValue())); \
		} \
		return *this; \
	} \
	SelfClass& set##NAME(const ::se::gui::Binding& _binding) \
	{ \
		setBinding( \
			bindingResolveDefaultValue( \
				_binding, \
				::se::gui::BindingMode::OneWay, \
				::se::gui::BindingSourceUpdate::PropertyChanged), \
			*getPropertyLink(#NAME)); \
		return *this; \
	} \
	SelfClass& set##NAME(const ::se::gui::ResourceKey& _key) \
	{ \
		const ::se::gui::IPropertyLink* link = getPropertyLink(#NAME); \
		if (initialized) \
		{ \
			resolveResource(::se::gui::UnresolvedResource{ _key, link }, *this); \
			return *this; \
		} \
		unresolvedResources.emplace_back(_key, link); \
		return *this; \
	} \
	const TYPE& get##NAME() const \
	{ \
		return property_##NAME.getValue(); \
	}

#define GUI_REGISTER_MEMBER_PROPERTY(NAME) \
	_meta.registerMemberProperty(#NAME, SelfClass::property_##NAME##Default, &SelfClass::property_##NAME);


	template<typename PropertyType>
	class MemberProperty
	{
	public:
		MemberProperty(const PropertyType& _default)
			: value(_default) {}

		PropertyPrecedence getPrecedence() const
		{
			return type;
		}
		const PropertyType& getValue() const
		{
			if (triggerValue)
				return *triggerValue;
			return value;
		}
		bool setValue(const PropertyType& _value, PropertyPrecedence _type)
		{
			if (type > _type)
				return false;
			if constexpr (boost::has_equal_to<PropertyType>::value)
			{
				// This is just an optimization, equals operator not strictly necessary
				if (value == _value)
					return false;
			}

			value = _value;
			type = _type;
			if (triggerType < _type)
			{
				// override
				triggerValue = nullptr;
			}
			return triggerValue == nullptr;
		}
		bool setTriggerValue(const std::any& _value, PropertyPrecedence _type)
		{
			if (type > _type || triggerType > _type)
				return false;

			triggerType = _type;
			if (!_value.has_value())
			{
				se_assert(triggerValue != nullptr);
				triggerValue = nullptr;
				triggerType = PropertyPrecedence::Default;
			}
			else
			{
				triggerValue = &std::any_cast<const PropertyType&>(_value);
			}
			return true;
		}

	private:
		PropertyPrecedence type = PropertyPrecedence::Default;
		PropertyPrecedence triggerType = PropertyPrecedence::Default;
		const PropertyType* triggerValue = nullptr;
		PropertyType value;
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
		void setValue(IPropertyHost& _host, const void* _value, PropertyPrecedence _type) const override
		{
			MemberProperty<PropertyType>& prop = getProperty(_host);
			if (prop.setValue(*static_cast<const PropertyType*>(_value), _type))
			{
				_host.propertyChanged(name, static_cast<const void*>(&prop.getValue()));
			}
		}
		void setTriggerValue(IPropertyHost& _host, const std::any& _value, PropertyPrecedence _type) const override
		{
			MemberProperty<PropertyType>& prop = getProperty(_host);
			if (prop.setTriggerValue(_value, _type))
			{
				_host.propertyChanged(name, static_cast<const void*>(&prop.getValue()));
			}
		}
		bool compareValue(const IPropertyHost& _host, const std::any& _value) const override
		{
			if constexpr (boost::has_equal_to<PropertyType>::value)
			{
				return getProperty(_host).getValue() == std::any_cast<const PropertyType&>(_value);
			}
			else
			{
				// compareValue obviously won't work without an equals operator,
				// but some properties are not meant to be used in systems that use this feature.
				log::warning("Cannot compare MemberProperty value '" + getName() + "'!");
				return false;
			}
		}
		void resetValue(IPropertyHost& _host, PropertyPrecedence _type) const override
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
