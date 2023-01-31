#pragma once

#include "stdafx.h" //
#include "PropertyHostMeta.h"


namespace se::gui
{
	class Mirror
	{
	public:
		Mirror()
		{
			registeredPropertyHosts.push_back(std::make_unique<PropertyHostMeta>(
				"IPropertyHost", nullptr, nullptr));
			registerPropertyHost<Element>();
		}

		template<typename HostType>
		void registerPropertyHost()
		{
			static_assert(std::is_base_of<IPropertyHost, HostType>::value,
				"registerPropertyHost HostType must inherit from IPropertyHost");

			const std::string className = HostType::getSelfClassName();
			if (findPropertyHostMeta(className))
			{
				log::error(className + " type already registered.");
				return;
			}

			const std::string baseName = HostType::getBaseClassName();
			PropertyHostMeta* const baseMeta = findPropertyHostMeta(baseName);
			if (!baseMeta)
			{
				log::error(className + "'s base class '" + baseName + "' type not registered.");
				return;
			}

			registeredPropertyHosts.push_back(
				std::make_unique<PropertyHostMeta>(className, &HostType::create, baseMeta));
			PropertyHostMeta* meta = registeredPropertyHosts.back().get();
			HostType::registerProperties(*meta);

			HostType::meta_propertyGetFn =
				[meta](const std::string& _name){ return meta->getPropertyLink(_name); };
			unregisters.push_back(std::make_unique<CallOnDelete>(
				[]{ HostType::meta_propertyGetFn = nullptr; }));
		}

		std::shared_ptr<IPropertyHost> createInstance(const std::string& _name)
		{
			if (const PropertyHostMeta* host = findPropertyHostMeta(_name))
			{
				return host->createFn();
			}
			log::error(_name + " type not registered.");
			return nullptr;
		}

	private:

		PropertyHostMeta* findPropertyHostMeta(const std::string& _selfName) const
		{
			for (auto&& elem : registeredPropertyHosts)
			{
				if (elem->name == _selfName)
					return elem.get();
			}
			return nullptr;
		}

		std::vector<std::unique_ptr<PropertyHostMeta>> registeredPropertyHosts;
		unique_ptr_vector<CallOnDelete> unregisters;
	};
}
