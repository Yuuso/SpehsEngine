#pragma once

#ifdef _DEBUG
//Generate function call when debug configuration is used
#include <string>
#define checkOpenGLErrors(_file, _line) spehs::debugOpenGLError(_file, _line)
namespace spehs
{
	extern void debugOpenGLError(const std::string &_file, int _line);
}
#else
//Generate no code!
#define checkOpenGLErrors(_file, _line) ((void)0)
#endif