#pragma once

#include <string_view>


// Key is omitted at compile time if not using key writer
#define se_writer(p_writer, p_value, p_key) \
do { \
	if constexpr (std::remove_reference<decltype(p_writer)>::type::getKeyEnabled()) \
	{ \
		se::keyWriter(p_writer, p_value, p_key); \
	} \
	else \
	{ \
		se::keylessWriter(p_writer, p_value); \
	} \
} while(false)

// Key is omitted at compile time if not using key reader
#define se_reader(p_reader, p_value, p_key) \
do { \
	if constexpr (std::remove_reference<decltype(p_reader)>::type::getKeyEnabled()) \
	{ \
		if (!se::keyReader(p_reader, p_value, p_key)) \
		{ \
			return false; \
		} \
	} \
	else \
	{ \
		if (!se::keylessReader(p_reader, p_value)) \
		{ \
			return false; \
		} \
	} \
} while(false)

namespace se
{
	namespace serial_detail
	{
		template<class> struct type_sink { typedef void type; }; // consumes a type, and makes it `void`
		template<class T> using type_sink_t = typename type_sink<T>::type;
		template<class Writer, class T, class = void>	struct has_free_writer : std::false_type {};
		template<class Writer, class T>					struct has_free_writer<Writer, T, type_sink_t<decltype(writer(std::declval<Writer&>(), std::declval<const T&>()), void())>> : std::true_type {};
		template<class Reader, class T, class = void>	struct has_free_reader : std::false_type {};
		template<class Reader, class T>					struct has_free_reader<Reader, T, type_sink_t<decltype(reader(std::declval<Reader&>(), std::declval<T&>()), bool())>> : std::true_type {};
	}

	template<typename Writer, typename Value>
	inline void keylessWriter(Writer& _writer, const Value& _value)
	{
		_writer.serial(_value);
	}
	template<typename Writer, typename Value>
	inline void keyWriter(Writer& _writer, const Value& _value, const std::string_view _key)
	{
		_writer.serial(_value, _key);
	}
	template<typename Reader, typename Value>
	inline bool keylessReader(Reader& _reader, Value& _value)
	{
		return _reader.serial(_value);
	}
	template<typename Reader, typename Value>
	inline bool keyReader(Reader& _reader, Value& _value, const std::string_view _key)
	{
		return _reader.serial(_value, _key);
	}
}
