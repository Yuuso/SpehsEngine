#pragma once

#include "stdafx.h" //
#include <any>


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
		using Getter = std::function<std::any(const IPropertyHost*)>;
		using Setter = std::function<void(IPropertyHost*, const std::any&)>;
		PropertyLink(PropertyMap& _props,
					 const std::string& _name,
					 const Getter _getter,
					 const Setter _setter,
					 const std::any& _defaultValue)
			: name(_name)
			, getProperty(_getter)
			, setProperty(_setter)
			, defaultValue(_defaultValue)
		{
			se_assert_m(_props.find(_name) == _props.end(), "Property '" + _name + "' already exists!");
			_props[_name] = this;
		}
		const std::string name;
		const Getter getProperty;
		const Setter setProperty;
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
		std::any getProperty(const std::string& _name) const
		{
			const PropertyMap& map = getProperties();
			auto it = map.find(_name);
			if (it != map.end())
			{
				return it->second->getProperty(this);
			}
			return {};
		}
		void setProperty(const std::string& _name, const std::any& _value)
		{
			PropertyMap& map = getProperties();
			auto it = map.find(_name);
			if (it != map.end())
			{
				it->second->setProperty(this, _value);
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