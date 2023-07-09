#pragma once

#include <string_view>
#include "SpehsEngine/Core/Serial/SerialKey.h"
#include "SpehsEngine/Core/RemoveCvref.h"

// Key is omitted at compile time if not using key serial
#define se_serial(p_serial, p_value, p_key) \
do { \
	if constexpr (std::remove_reference<decltype(p_serial)>::type::getKeyEnabled()) \
	{ \
		const se::SerialTypeId p_serialTypeId = se::getSerialTypeId<se::remove_cvref<decltype(p_value)>::type>(); \
		const uint32_t p_serialKeyValue = se::Murmur3::impl2(p_key, p_serialTypeId.value); \
		const se::SerialKey<se::remove_cvref<decltype(p_value)>::type> p_serialKey(p_serialKeyValue); \
		if (!se::keySerial(p_serial, p_value, p_serialKey)) \
			return false; \
	} \
	else \
	{ \
		if (!se::keylessSerial(p_serial, p_value)) \
			return false; \
	} \
} while(false)

namespace se
{
	template<typename U>
	struct Serial
	{
		// User types must implement this for basic serialization
		template<typename Serial, typename URef>	static bool serial(Serial& _serial, URef _value);

		// OPTIONAL: User types must implement these for abstract serialization (used with std::unique_ptr<UserType> serialization)
		template<typename Target, typename Id>		static void cast(Target& _target, const Id& _id);
		template<typename Id>						static Id identify(const U& _value);
	};

	// OPTIONAL: User types must implement this for abstract serialization, this must match the type returned from Serial<UserType>::identity().
	template<typename ... Args>
	struct SerialId
	{
	};

	// Serial tag types are used to allow making serial functions for template classes, see stl container serial functions as an example.
	template<typename ... Args>
	struct SerialTag
	{
	};
	template<typename T>
	struct SerialTag<T>
	{
		typedef T type;
	};

	template<typename Serial, typename Value>
	inline bool keylessSerial(Serial& _serial, Value& _value)
	{
		return _serial.serial(_value);
	}
	template<typename Serial, typename Value>
	inline bool keylessSerial(Serial& _serial, const Value& _value)
	{
		return _serial.serial(_value);
	}
	template<typename Serial, typename Value>
	inline bool keySerial(Serial& _serial, Value& _value, const SerialKey<Value> _serialKey)
	{
		return _serial.serial(_value, _serialKey);
	}
	template<typename Serial, typename Value>
	inline bool keySerial(Serial& _serial, const Value& _value, const SerialKey<Value> _serialKey)
	{
		return _serial.serial(_value, _serialKey);
	}
}
