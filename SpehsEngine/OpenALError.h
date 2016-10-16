
#pragma once

#include <string>

struct ALCdevice;


namespace spehs
{
	extern void checkOpenALError(const std::string &_file, int _line);
	extern void checkOpenALCError(ALCdevice* _device, const std::string &_file, int _line);
}