#pragma once
#include <string>

/*
This class handles all errors and exceptions
*/

namespace se
{
	namespace exceptions
	{
		extern void warning(const std::string warningString);
		extern void unexpectedError(const std::string errorString);//Program execution may continue
		extern void fatalError(const std::string errorString);//Program must terminate execution
	}
}
