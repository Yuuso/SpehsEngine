
#pragma once

#if defined(FINAL_RELEASE)
	// Generate no code!
	#define se_assert(_file) ((void)0)
	#define se_assert_m(__expression, __message) ((void)0)
#else
	#include <string>

	namespace se
	{
		// Returns true when user want to debug
		bool AssertMessageBox(const std::string& _message, bool& _ignore);
	}

	#define se_assert(__expression) \
	{ \
		if (!(__expression)) { \
			static bool __ignore = false; \
			if (!__ignore) { \
				static const std::string __assertMessage("Expression: " #__expression "\nFILE: " __FILE__ "\nLINE: " + std::to_string(__LINE__)); \
				if (se::AssertMessageBox(__assertMessage, __ignore)) \
					__debugbreak(); \
			} \
		} \
	}

	#define se_assert_m(__expression, __message) \
	{ \
		if (!(__expression)) { \
			static bool __ignore = false; \
			if (!__ignore) { \
				static const std::string __assertMessage("Expression: " #__expression "\nFILE: " __FILE__ "\nLINE: " + std::to_string(__LINE__) +  "\n" + __message); \
				if (se::AssertMessageBox(__assertMessage, __ignore)) \
					__debugbreak(); \
			} \
		} \
	}
#endif
