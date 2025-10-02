#pragma once


namespace se
{
	namespace detail
	{
		// Assert type compatibility between V and B, define the use type for the operation
		template<typename V, typename B>
		struct Underlying
		{
			static_assert(std::is_enum<V>::value || std::is_integral<V>::value, " Type V must be enum or integral.");
			static_assert(std::is_enum<B>::value || std::is_integral<B>::value, " Type B must be enum or integral.");
			static_assert(sizeof(V) == sizeof(B), "Types V and B must be of the same size");
			static_assert((!std::is_enum<V>::value || !std::is_enum<B>::value) || std::is_same<V, B>::value, "Should not mix different enum types when performing bit operations.");
			template<typename U, bool IsEnum> struct Impl {};
			template<typename U> struct Impl<U, false> { using type = typename U; };
			template<typename U> struct Impl<U, true> { using type = typename std::underlying_type<U>::type; };
			using type = typename Impl<V, std::is_enum<V>::value>::type;
		};
	}

	// Returns all given bits or-ed together
	template<typename V>
	constexpr V orBits(const V _a, const V _b)
	{
		using U = typename detail::Underlying<V, V>::type;
		return V(U(_a) | U(_b));
	}
	template<typename V, typename ... Args>
	constexpr V orBits(const V _a, const V _b, Args &&... _args)
	{
		using U = typename detail::Underlying<V, V>::type;
		return orBits(orBits(_a, _b), std::forward<Args>(_args)...);
	}

	// Returns all given bits and-ed together
	template<typename V>
	constexpr V andBits(const V _a, const V _b)
	{
		using U = typename detail::Underlying<V, V>::type;
		return V(U(_a) & U(_b));
	}
	template<typename V, typename ... Args>
	constexpr V andBits(const V _a, const V _b, Args &&... _args)
	{
		using U = typename detail::Underlying<V, V>::type;
		return andBits(andBits(_a, _b), std::forward<Args>(_args)...);
	}

	// Enables all given bits in the variable
	template<typename V, typename B>
	constexpr void enableBit(V& variable, const B bits)
	{
		using U = typename detail::Underlying<V, B>::type;
		variable = V(U(variable) | U(bits));
	}

	// Disables all given bits in the variable
	template<typename V, typename B>
	constexpr void disableBit(V& variable, const B bits)
	{
		using U = typename detail::Underlying<V, B>::type;
		variable = V(U(variable) & ~U(bits));
	}

	// Toggles all given bits in the variable
	template<typename V, typename B>
	constexpr void toggleBit(V& variable, const B bits)
	{
		using U = typename detail::Underlying<V, B>::type;
		variable = V(U(variable) ^ U(bits));
	}

	// Returns true if all given bits are enabled in the variable
	template<typename V, typename B>
	constexpr bool checkBit(const V variable, const B bits)
	{
		using U = typename detail::Underlying<V, B>::type;
		return (V(U(variable) & U(bits)) == V(bits));
	}

	// Sets all given bits the variable to either 1 or 0
	template<typename V, typename B>
	constexpr void setBit(V& variable, const B bits, const bool enabled)
	{
		if (enabled)
			enableBit<V, B>(variable, bits);
		else
			disableBit<V, B>(variable, bits);
	}
}
