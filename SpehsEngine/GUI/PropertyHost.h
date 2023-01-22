#pragma once

#include "stdafx.h" //


namespace se::gui
{

#define GUI_PROPERTY_CLASS(CLASS) \
private: \
	using SelfClass = CLASS; \
	inline static ::se::gui::PropertyMap property_Map; \
	inline ::se::gui::PropertyMap& getProperties() override { return property_Map; } \
	inline const ::se::gui::PropertyMap& getProperties() const override { return property_Map; } \

	class IPropertyHost;
	class PropertyLink;
	using PropertyMap = std::unordered_map<std::string, const PropertyLink*>;

	class PropertyLink
	{
	public:
		using PropertyGetFn = std::function<std::any(const IPropertyHost*)>;
		using PropertySetFn = std::function<void(IPropertyHost*, const std::any&)>;
		using PropertyCompFn = std::function<bool(IPropertyHost*, const std::any&)>;

		PropertyLink(PropertyMap& _props,
					 const std::string& _name,
					 const PropertyGetFn _getter,
					 const std::any& _defaultValue)
			: PropertyLink(_props, _name, _getter, nullptr, nullptr, nullptr, _defaultValue) {}
		PropertyLink(PropertyMap& _props,
					 const std::string& _name,
					 const PropertyGetFn _getter,
					 const PropertySetFn _setter,
					 const std::any& _defaultValue)
			: PropertyLink(_props, _name, _getter, _setter, nullptr, nullptr, _defaultValue) {}
		PropertyLink(PropertyMap& _props,
					 const std::string& _name,
					 const PropertyGetFn _getter,
					 const PropertySetFn _setter,
					 const PropertyCompFn _comp,
					 const PropertySetFn _trigger,
					 const std::any& _defaultValue)
			: name(_name)
			, getProperty(_getter)
			, setProperty(_setter)
			, compProperty(_comp)
			, setPropertyTrigger(_trigger)
			, defaultValue(_defaultValue)
		{
			se_assert_m(_props.find(_name) == _props.end(), "Property '" + _name + "' already exists!");
			_props[_name] = this;
		}

		const std::string name;
		const PropertyGetFn getProperty;
		const PropertySetFn setProperty;
		const PropertyCompFn compProperty;
		const PropertySetFn setPropertyTrigger;
		const std::any defaultValue;
	};

	class SE_INTERFACE IPropertyHost
	{
	public:
		virtual PropertyMap& getProperties() = 0;
		virtual const PropertyMap& getProperties() const = 0;

	public:
		bool hasProperty(const std::string& _name) const
		{
			const PropertyMap& map = getProperties();
			return map.find(_name) != map.end();
		}
		const PropertyLink* getPropertyLink(const std::string& _name)
		{
			const PropertyMap& map = getProperties();
			auto it = map.find(_name);
			if (it != map.end())
			{
				return it->second;
			}
			return nullptr;
		}
		std::any getProperty(const std::string& _name)
		{
			if (const PropertyLink* link = getPropertyLink(_name))
			{
				return link->getProperty(this);
			}
			return {};
		}
		void setProperty(const std::string& _name, const std::any& _value)
		{
			if (const PropertyLink* link = getPropertyLink(_name))
			{
				link->setProperty(this, _value);
			}
		}
		bool compareProperty(const std::string& _name, const std::any& _value)
		{
			if (const PropertyLink* link = getPropertyLink(_name))
			{
				return link->compProperty(this, _value);
			}
			return false;
		}
		void setPropertyTriggerValue(const std::string& _name, const std::any& _value)
		{
			if (const PropertyLink* link = getPropertyLink(_name))
			{
				link->setPropertyTrigger(this, _value);
			}
		}

	public:
		Connection onPropertyChanged(const std::function<void(const std::string&, const std::any&)>& func)
		{
			return propertyChangedSignal.connect(func);
		}
	protected:
		Signal<void(const std::string&, const std::any&)> propertyChangedSignal;
	};
}