#pragma once

#include "SpehsEngine/GUI/PropertyLink.h"


namespace se::gui
{
#define GUI_DATA_PROPERTY(TYPE, NAME, VALUE_SIG) \
public: \
	const TYPE& get##NAME() const \
	{ \
		return VALUE_SIG; \
	} \
	void set##NAME(const TYPE& _value) \
	{ \
		VALUE_SIG = _value; \
		propertyChanged(#NAME, static_cast<const void*>(&VALUE_SIG)); \
	}

#define GUI_REGISTER_DATA_PROPERTY(NAME, GETFN, SETFN) \
	_meta.registerDataProperty(#NAME, GETFN, SETFN);


	template<typename PropertyType, typename HostType>
	using DataPropertyGetFn = const PropertyType&(HostType::*)() const;

	template<typename PropertyType, typename HostType>
	using DataPropertySetFn = void(HostType::*)(const PropertyType&);

	template<typename PropertyType, typename HostType>
	class DataPropertyLink : public IPropertyLink
	{
	public:
		DataPropertyLink(
				const std::string& _name,
				const DataPropertyGetFn<PropertyType, HostType>& _getFn,
				const DataPropertySetFn<PropertyType, HostType>& _setFn)
			: name(_name)
			, getFn(_getFn)
			, setFn(_setFn) {}

		const void* getValue(const IPropertyHost& _host) const override
		{
			return static_cast<const void*>(&(static_cast<const HostType&>(_host).*getFn)());
		}
		void setValue(IPropertyHost& _host, const void* _value, PropertyValueType) const override
		{
			if (setFn == nullptr)
			{
				log::warning("Cannot set '" + name + "' DataProperty value, it is read-only.");
				return;
			}
			(static_cast<HostType&>(_host).*setFn)(*static_cast<const PropertyType*>(_value));
			// NOTE: propertyChanged is the host type's responsibility!
		}
		void setTriggerValue(IPropertyHost&, const std::any&, PropertyValueType) const override
		{
			se_assert_m(false, "Cannot set trigger value on a DataProperty!");
		}
		bool compareValue(const IPropertyHost& _host, const std::any& _value) const override
		{
			return (static_cast<const HostType&>(_host).*getFn)() == std::any_cast<const PropertyType&>(_value);
		}
		void resetValue(IPropertyHost&, PropertyValueType) const override
		{
			se_assert_m(false, "Cannot reset the value of a DataProperty!");
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
		const std::string name;
		const DataPropertyGetFn<PropertyType, HostType> getFn;
		const DataPropertySetFn<PropertyType, HostType> setFn;
	};
}
