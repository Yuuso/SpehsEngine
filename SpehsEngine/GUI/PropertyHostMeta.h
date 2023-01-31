#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/DataProperty.h"
#include "SpehsEngine/GUI/MemberProperty.h"


namespace se::gui
{
	class Mirror;

#define GUI_REFLECT(SELF, BASE) \
private: \
	using SelfClass = SELF; \
	using BaseClass = BASE; \
public: \
	inline static std::string getSelfClassName() { return ::se::gui::simplifyClassName(#SELF); } \
	inline static std::string getBaseClassName() { return ::se::gui::simplifyClassName(#BASE); } \
	inline static std::shared_ptr<SelfClass> create() { return std::make_shared<SelfClass>(); } \
	virtual const ::se::gui::IPropertyLink* getPropertyLink(const std::string& _name) override \
	{ \
		if (meta_propertyGetFn == nullptr) ::se::log::fatal(#SELF " type not registered!"); \
		return meta_propertyGetFn(_name); \
	} \
private: \
	friend class ::se::gui::Mirror; \
	inline static std::function<const ::se::gui::IPropertyLink*(const std::string&)> meta_propertyGetFn = nullptr; \
	static void registerProperties(::se::gui::PropertyHostMeta& _meta)

	inline std::string simplifyClassName(const char* _name)
	{
		std::string result = _name;
		const size_t pos = result.find_last_of(':');
		if (pos != std::string::npos)
		{
			return result.substr(pos + 1);
		}
		return result;
	}


	class IPropertyHost;
	using CreatePropertyHostFn = std::function<std::shared_ptr<IPropertyHost>()>;

	class PropertyHostMeta
	{
	public:
		PropertyHostMeta(
				const std::string& _name,
				const CreatePropertyHostFn& _createFn,
				PropertyHostMeta* const _base)
			: name(_name)
			, createFn(_createFn)
			, baseMeta(_base) {}

		const IPropertyLink* getPropertyLink(const std::string& _name) const
		{
			for (auto&& prop : properties)
			{
				if (prop->getName() == _name)
					return prop.get();
			}
			if (baseMeta)
				return baseMeta->getPropertyLink(_name);
			return nullptr;
		}

		template<typename PropertyType, typename HostType>
		void registerMemberProperty(
			const std::string& _name,
			const PropertyType& _defaultValue,
			const MemberPropertyPtr<PropertyType, HostType>& _ptr)
		{
			if (getPropertyLink(_name))
			{
				log::error(_name + " property already registered.");
				return;
			}

			properties.emplace_back(
				std::make_unique<const MemberPropertyLink<PropertyType, HostType>>(
					_name, _defaultValue, _ptr));
		}

		template<typename PropertyType, typename HostType>
		void registerDataProperty(
			const std::string& _name,
			const DataPropertyGetFn<PropertyType, HostType>& _getFn,
			const DataPropertySetFn<PropertyType, HostType>& _setFn)
		{
			if (getPropertyLink(_name))
			{
				log::error(_name + " property already registered.");
				return;
			}

			properties.emplace_back(
				std::make_unique<const DataPropertyLink<PropertyType, HostType>>(
					_name, _getFn, _setFn));
		}

		const std::string name;
		const CreatePropertyHostFn createFn;

	private:
		PropertyHostMeta* const baseMeta = nullptr;
		std::vector<std::unique_ptr<const IPropertyLink>> properties;
	};
}
