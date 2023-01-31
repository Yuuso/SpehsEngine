#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/PropertySetter.h"
#include "SpehsEngine/GUI/Trigger.h"


namespace se::gui
{
	class Resource
	{
	public:
		Resource();
		Resource(std::string_view _key)
			: key(_key) {}
		std::string key;
	};

	class SE_INTERFACE IResource
	{
	public:
		virtual ~IResource() = default;
		IResource(std::string_view _key)
			: key(_key) {}
		const std::string key;
	};

	class UnresolvedResource
	{
	public:
		UnresolvedResource(const Resource& _resource, const std::function<void(const IResource*)>& _setter)
			: resource(_resource), setter(_setter) {}
		const Resource resource;
		const std::function<void(const IResource*)> setter;
	};


	class Int : public IResource
	{
	public:
		Int(std::string_view _key, int _value)
			: IResource(_key), value(_value) {}
		int value = 0;
	};

	class Style : public IResource
	{
	public:
		Style(std::string_view _key)
			: IResource(_key) {}
		Resource basedOn;
		std::string targetName;
		std::vector<PropertySetter> setters;
		std::vector<PropertyTrigger> propertyTriggers;
		std::vector<EventTrigger> eventTriggers;
	};
}
