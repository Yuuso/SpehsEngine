#pragma once

#ifdef _DEBUG
//Generate function call when debug configuration is used
#include <string>
#define checkOpenGLErrors(_file, _line) se::rendering::debugOpenGLError(_file, _line)
namespace se
{
	namespace rendering
	{
		extern void debugOpenGLError(const std::string &_file, int _line);
	}
}
#else
//Generate no code!
#define checkOpenGLErrors(_file, _line) ((void)0)
#endif