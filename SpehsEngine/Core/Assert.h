#pragma once
#ifdef _DEBUG
#define se_assert(expression) \
{ \
	if (!(expression)) \
		se::log::error("Assert failed.\nExpression: " #expression "\nFILE: " __FILE__ "\nLINE: " + std::to_string(__LINE__)); \
}
#else
#ifdef NDEBUG
//Generate no code!
#define se_assert(_file) ((void)0)
#else
#error Neither _DEBUG nor NDEBUG is defined!
#endif
#endif