#pragma once

#include <type_traits>
#include <ranges>


namespace se
{
	//
	// Useful for iterating enum classes:
	// for (SomeEnum e : enumRange(SomeEnum::First, SomeEnum::Last))
	//
	template<typename EnumT>
	constexpr inline auto enumRange(EnumT first, EnumT last)
	{
		return std::views::iota(
			static_cast<std::underlying_type_t<EnumT>>(first),
			static_cast<std::underlying_type_t<EnumT>>(last) + 1)
			| std::views::transform([](auto e){ return static_cast<EnumT>(e); });
	};

	//
	// Useful for iterating enum classes:
	// for (SomeEnum e : enumRange(SomeEnum::Count))
	// NOTE: count value is not included in the range!
	//
	template<typename EnumT>
	constexpr inline auto enumRange(EnumT count)
	{
		return std::views::iota(
			static_cast<std::underlying_type_t<EnumT>>(0),
			static_cast<std::underlying_type_t<EnumT>>(count))
			| std::views::transform([](auto e){ return static_cast<EnumT>(e); });
	};
}
