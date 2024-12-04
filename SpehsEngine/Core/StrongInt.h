#pragma once


#define SE_STRONG_INT_BASIC_IMPL(p_IntType, p_TypeName, p_DefaultValue) \
	static_assert(std::is_integral<p_IntType>::value, "p_IntType must be an integral type."); \
	typedef p_IntType ValueType;\
	struct HashFunctor \
	{ \
		constexpr size_t operator()(const p_TypeName& strongInt) const \
		{ \
			if constexpr (sizeof(strongInt.value) > sizeof(size_t)) \
				return se::Murmur3::impl((const char*)&strongInt.value, sizeof(strongInt.value), 0); \
			else \
				return size_t(strongInt.value); \
		} \
	}; \
	constexpr p_TypeName() noexcept = default; \
	constexpr p_TypeName(const p_IntType _value) noexcept : value(_value) {} \
	constexpr p_TypeName(const p_TypeName &_other) noexcept : value(_other.value) {} \
	constexpr bool operator==(const p_TypeName& other) const noexcept { return value == other.value; } \
	constexpr bool operator!=(const p_TypeName& other) const noexcept { return value != other.value; } \
	constexpr void operator=(const p_TypeName& other) noexcept { value = other.value; }; \
	constexpr void operator=(p_TypeName&& other) noexcept { value = other.value; } \
	constexpr p_TypeName operator++(int) noexcept { return p_TypeName(value++); } \
	constexpr operator p_IntType() const noexcept { return value; } \
	p_IntType value = p_DefaultValue;

#define SE_STRONG_INT_INVALID_IMPL(p_InvalidValue) \
	constexpr bool isValid() const noexcept { return value != p_InvalidValue; } \
	constexpr explicit operator bool() const noexcept { return value != p_InvalidValue; } \

#define SE_STRONG_INT(p_IntType, p_TypeName, p_InvalidValue) \
struct p_TypeName \
{ \
	SE_STRONG_INT_INVALID_IMPL(p_InvalidValue) \
	SE_STRONG_INT_BASIC_IMPL(p_IntType, p_TypeName, p_InvalidValue) \
};

#define SE_STRONG_INT_WITHOUT_INVALID(p_IntType, p_TypeName, p_DefaultValue) \
struct p_TypeName \
{ \
	SE_STRONG_INT_BASIC_IMPL(p_IntType, p_TypeName, p_DefaultValue) \
};

// Must be used from the global namespace
#define SE_STRONG_INT_STD(p_TypeName) \
namespace std \
{ \
	template<> struct hash<p_TypeName> \
	{ \
		constexpr size_t operator()(const p_TypeName& p_value) const \
		{ \
			return p_TypeName::HashFunctor()(p_value); \
		} \
	}; \
	template<> struct less<p_TypeName> \
	{ \
		constexpr size_t operator()(const p_TypeName& a, const p_TypeName& b) const \
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
