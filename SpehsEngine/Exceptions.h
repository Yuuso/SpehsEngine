#pragma once
#include <string>

/*
This class handles all errors and exceptions
*/

namespace spehs
{
	extern void fatalError(std::string errorString);
	extern void unexpectedError(std::string errorString);
	extern void warning(std::string warningString);
}
