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
	bool operator==(const p_TypeName& other) const { return value == other.value; } \
	bool operator!=(const p_TypeName& other) const { return value != other.value; } \
	p_TypeName operator++(int) { return p_TypeName(value++); } \
	operator bool() const { return value != p_InvalidValue; } \
	operator p_IntType() const { return value; } \
	p_IntType value = p_InvalidValue; \
};
