#pragma once
#include <string>

namespace spehs
{
	namespace log
	{
		void info(const std::string& message);
		void warning(const std::string& message);
		void error(const std::string& message);
		void info(const char* message);
		void warning(const char* message);
		void error(const char* message);
	}
}

//Spehs assert
#ifdef _DEBUG
#define SPEHS_ASSERT(expression) { if (!(expression)) spehs::log::error("SPEHS_ASSERT failed.\nExpression: " #expression "\nFILE: " __FILE__ "\nLINE: " + std::to_string(__LINE__)); }
#else
//Generate no code!
#define SPEHS_ASSERT(_file, _line) ((void)0)
#endif