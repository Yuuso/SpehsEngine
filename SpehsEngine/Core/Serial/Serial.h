#pragma once

#include <string_view>


// Key is omitted at compile time if not using key serial
#define se_serial(p_serial, p_value, p_key) \
do { \
	if constexpr (std::remove_reference<decltype(p_serial)>::type::getKeyEnabled()) \
	{ \
		if (!se::keySerial(p_serial, p_value, p_key)) \
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
	namespace serial_detail
	{
	}

	template<typename T>
	struct Serial
	{
		template<typename Serial, typename T>
		static bool impl(Serial& _serial, T _value);
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

	//template<typename T, typename Serial, typename TRef>
	//static bool serialImpl(Serial& _serial, TRef _value);

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
	inline bool keySerial(Serial& _serial, Value& _value, const std::string_view _key)
	{
		return _serial.serial(_value, _key);
	}
	template<typename Serial, typename Value>
	inline bool keySerial(Serial& _serial, const Value& _value, const std::string_view _key)
	{
		return _serial.serial(_value, _key);
	}
}
