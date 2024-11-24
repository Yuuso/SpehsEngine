#pragma once

/*
	MSVC:
	Using variadic arguments as arguments to further macros is tricky.
	They need to be expanded by placing the entire macro call inside this expansion macro.
	Otherwise all of the arguments will be passed down to the next macro as a single argument.
	MACRO1(a, b, c, ...) SE_EXPAND(MACRO(1, 2, 3, __VA_ARGS__))
*/
#define SE_EXPAND(x) x

// Combining arguments
#define SE_PP_CONCAT(A, B) SE_PP_CONCAT_IMPL(A, B)
#define SE_PP_CONCAT_IMPL(A, B) A##B

// Obtaining argument count
#define SE_PP_ARG_COUNT_IMPL(_8, _7, _6, _5, _4, _3, _2, _1, N, ...) N
#define SE_PP_ARG_COUNT(...) SE_EXPAND(SE_PP_ARG_COUNT_IMPL(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1))

// Obtaining argument by index
#define SE_PP_ARG_N(N, ...) SE_EXPAND(SE_PP_CONCAT(SE_PP_ARG_, N)(__VA_ARGS__))
#define SE_PP_ARG_1(_1, ...) _1
#define SE_PP_ARG_2(_1, _2, ...) _2
#define SE_PP_ARG_3(_1, _2, _3, ...) _3
#define SE_PP_ARG_4(_1, _2, _3, _4, ...) _4
#define SE_PP_ARG_5(_1, _2, _3, _4, _5, ...) _5
#define SE_PP_ARG_6(_1, _2, _3, _4, _5, _6, ...) _6
#define SE_PP_ARG_7(_1, _2, _3, _4, _5, _6, _7, ...) _7
#define SE_PP_ARG_8(_1, _2, _3, _4, _5, _6, _7, _8, ...) _8

// Obtain the last argument
#define SE_PP_ARG_LAST(...) SE_EXPAND(SE_PP_ARG_N(SE_PP_ARG_COUNT(__VA_ARGS__), __VA_ARGS__,,,,,,,,,))

// Obtain the number of elements used in a "for each" style macro at compile time
#define SE_FOR_EACH_CONSTEXPR_COUNT_IMPL(...)+1
#define SE_FOR_EACH_CONSTEXPR_COUNT(p_ForEachMacro) 0 p_ForEachMacro(SE_FOR_EACH_CONSTEXPR_COUNT_IMPL)
