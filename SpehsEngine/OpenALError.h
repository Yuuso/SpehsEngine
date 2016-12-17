
#pragma once

#include <string>


struct ALCdevice_struct;
typedef struct ALCdevice_struct ALCdevice;


namespace spehs
{
	extern void checkOpenALErrors(const std::string &_file, int _line);
	extern void checkOpenALCError(ALCdevice* _device, const std::string &_file, int _line);
}