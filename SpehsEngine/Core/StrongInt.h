#pragma once
#include <stdint.h>
#include "SpehsEngine/Core/ReadBuffer.h"
#include "SpehsEngine/Core/WriteBuffer.h"

#define SE_STRONG_INT(p_IntType, p_TypeName, p_InvalidValue) \
struct p_TypeName \
{ \
	static_assert(std::is_integral<p_IntType>::value, "p_IntType must be an integral type."); \
	typedef p_IntType ValueType;\
	struct HashFunctor \
	{ \
		size_t operator()(const p_TypeName& strongInt) const \
		{ \
			return std::hash<p_IntType>()(strongInt.value); \
		} \
	}; \
	void write(se::WriteBuffer& writeBuffer) const \
	{ \
		writeBuffer.write(value); \
	} \
	bool read(se::ReadBuffer& readBuffer) \
	{ \
		se_read(readBuffer, value); \
		return true; \
	} \
	bool isValid() const { return value != p_InvalidValue; } \
	p_TypeName() = default; \
	p_TypeName(const p_IntType _value) : value(_value) {} \
	p_TypeName(const p_TypeName &_other) : value(_other.value) {} \
	bool operator==(const p_TypeName& other) const { return value == other.value; } \
	bool operator!=(const p_TypeName& other) const { return value != other.value; } \
	void operator=(const p_TypeName& other) { value = other.value; }; \
	void operator=(p_TypeName&& other) { value = other.value; } \
	p_TypeName operator++(int) { return p_TypeName(value++); } \
	explicit operator bool() const { return value != p_InvalidValue; } \
	operator p_IntType() const { return value; } \
	p_IntType value = p_InvalidValue; \
};

// Must be used from the global namespace
#define SE_STRONG_INT_STD(p_TypeName) \
namespace std \
{ \
	template<> struct hash<p_TypeName> \
	{ \
		size_t operator()(const p_TypeName& p_value) const \
		{ \
			return p_TypeName::HashFunctor()(p_value); \
		} \
	}; \
	template<> struct less<p_TypeName> \
	{ \
		size_t operator()(const p_TypeName& a, const p_TypeName& b) const \
		{ \
			return p_TypeName::HashFunctor()(a) < p_TypeName::HashFunctor()(b); \
		} \
	}; \
} \
template<> template<typename S, typename T> \
static bool se::Serial<p_TypeName>::serial(S& _serial, T _strongInt) \
{ \
	se_serial(_serial, _strongInt.value, "i"); \
	return true; \
}
