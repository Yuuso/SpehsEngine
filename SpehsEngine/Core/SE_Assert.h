#pragma once
#ifdef FINAL_RELEASE
//Generate no code!
#define se_assert(_file) ((void)0)
#else
#define se_assert(expression) \
{ \
	if (!(expression)) \
		se::log::error("Assert failed.\nExpression: " #expression "\nFILE: " __FILE__ "\nLINE: " + std::to_string(__LINE__)); \
}
#endif