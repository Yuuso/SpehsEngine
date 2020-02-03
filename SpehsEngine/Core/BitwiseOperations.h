#pragma once

#include <type_traits>


namespace se
{
	template<typename Integer>
	inline typename std::enable_if<std::is_integral<Integer>::value, void>::type enableBit(Integer& variable, const Integer bits)
	{
		variable |= bits;
	}

	template<typename Enum>
	inline typename std::enable_if<std::is_enum<Enum>::value, void>::type enableBit(Enum& variable, const Enum bits)
	{
		variable = Enum(std::underlying_type<Enum>::type(variable) | std::underlying_type<Enum>::type(bits));
	}

	template<typename Integer>
	inline typename std::enable_if<std::is_integral<Integer>::value, void>::type disableBit(Integer& variable, const Integer bits)
	{
		variable &= ~bits;
	}

	template<typename Enum>
	inline typename std::enable_if<std::is_enum<Enum>::value, void>::type disableBit(Enum& variable, const Enum bits)
	{
		variable = Enum(std::underlying_type<Enum>::type(variable) & ~std::underlying_type<Enum>::type(bits));
	}

	template<typename Integer>
	inline typename std::enable_if<std::is_integral<Integer>::value, void>::type toggleBit(Integer& variable, const Integer bits)
	{
		variable ^= bits;
	}

	template<typename Enum>
	inline typename std::enable_if<std::is_enum<Enum>::value, void>::type toggleBit(Enum& variable, const Enum bits)
	{
		variable = Enum(std::underlying_type<Enum>::type(variable) ^ std::underlying_type<Enum>::type(bits));
	}

	/*
		Returns true if all of the specified bits are enabled.
	*/
	template<typename Integer>
	inline typename std::enable_if<std::is_integral<Integer>::value, bool>::type checkBit(const Integer variable, const Integer bits)
	{
		return (variable & bits) == bits;
	}

	/*
		Returns true if all of the specified bits are enabled.
	*/
	template<typename Enum>
	inline typename std::enable_if<std::is_enum<Enum>::value, bool>::type checkBit(const Enum variable, const Enum bits)
	{
		return (std::underlying_type<Enum>::type(variable) & std::underlying_type<Enum>::type(bits)) == std::underlying_type<Enum>::type(bits);
	}
}