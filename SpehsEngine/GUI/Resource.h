#pragma once

#include "stdafx.h"
#include "SpehsEngine/GUI/PropertySetter.h"


namespace se::gui
{
	using ResourceKey = std::variant<uint32_t, std::string>;

	inline bool operator==(const ResourceKey& _key1, const ResourceKey& _key2)
	{
		if (_key1.index() != _key2.index())
			return false;
		switch (_key1.index())
		{
			case 0: return std::get<0>(_key1) == std::get<0>(_key2);
			case 1: return std::get<1>(_key1) == std::get<1>(_key2);
		}
		se_assert(false);
		return false;
	}
	inline std::string toString(const ResourceKey& _key)
	{
		switch (_key.index())
		{
			case 0: return std::to_string(std::get<uint32_t>(_key));
			case 1: return std::get<std::string>(_key);
		}
		se_assert(false);
		return "";
	}

	class Resource
	{
	public:
		Resource(const std::string& _key)
			: key(_key) {}
		Resource(uint32_t _key)
			: key(_key) {}
		const ResourceKey key;
	};

	class UnresolvedResource
	{
	public:
		UnresolvedResource(const Resource& _resource, const IPropertyLink* _target)
			: resource(_resource), target(_target) {}
		const Resource resource;
		const IPropertyLink* target = nullptr;
	};


	struct ResourceTargetName			{ const std::string value; };
	struct ResourceTargetType			{ const std::string value; };
	struct ResourceTargetPropertyName	{ const std::string value; };

	class SE_INTERFACE IResource
	{
	public:
		virtual ~IResource() = default;
		IResource(const std::string& _key)
			: key(_key) {}
		IResource(uint32_t _key)
			: key(_key) {}
		IResource(ResourceTargetName _name, ResourceTargetPropertyName _prop)
			: targetElementName(_name), targetPropertyName(_prop) {}
		IResource(ResourceTargetType _type, ResourceTargetPropertyName _prop)
			: targetElementType(_type), targetPropertyName(_prop) {}

		virtual void applyToProperty(
			const IPropertyLink*, IPropertyHost&, PropertyValueType) const
		{
			log::warning("Cannot apply resource '" + toString(key) + "' to a property!");
		}

		const ResourceKey key;
		const ResourceTargetName targetElementName;
		const ResourceTargetType targetElementType;
		const ResourceTargetPropertyName targetPropertyName;
	};

	template<typename Type>
	class ValueResource : public IResource
	{
	public:
		virtual ~ValueResource() = default;
		ValueResource(const std::string& _key, const Type& _value)
			: IResource(_key), value(_value) {}
		ValueResource(uint32_t _key, const Type& _value)
			: IResource(_key), value(_value) {}
		ValueResource(ResourceTargetName _name, ResourceTargetPropertyName _prop, const Type& _value)
			: IResource(_name, _prop), value(_value) {}
		ValueResource(ResourceTargetType _type, ResourceTargetPropertyName _prop, const Type& _value)
			: IResource(_type, _prop), value(_value) {}

		virtual void applyToProperty(
			const IPropertyLink* _link,
			IPropertyHost& _host,
			PropertyValueType _type) const override
		{
			if (typeid(value) != _link->getType())
			{
				log::warning("Cannot apply resource '" + toString(key) +
					"' to property '" + _link->getName() + "'");
				return;
			}
			_link->setValue(_host, static_cast<const void*>(&value), _type);
		}

		const Type value{};
	};
}
