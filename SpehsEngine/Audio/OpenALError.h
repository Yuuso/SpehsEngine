
#pragma once

#include <string>


struct ALCdevice_struct;
typedef struct ALCdevice_struct ALCdevice;


namespace se
{
}

#ifdef _DEBUG
//Generate function call when debug configuration is used
#include <string>
#define checkOpenALErrors(_file, _line) se::debugOpenALErrors(_file, _line);
#define checkOpenALCError(_device, _file, _line) se::debugOpenALCError(_device, _file, _line);
namespace se
{
	extern void debugOpenALErrors(const std::string &_file, int _line);
	extern void debugOpenALCError(ALCdevice* _device, const std::string &_file, int _line);
}
#else
//Generate no code!
#define checkOpenALErrors(_file, _line) ((void)0)
#define checkOpenALCError(_device, _file, _line) ((void)0)
#endif