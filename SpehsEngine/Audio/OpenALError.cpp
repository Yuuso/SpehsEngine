#ifdef _DEBUG
#include "SpehsEngine/Audio/OpenALError.h"

#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>

#define STOP_AFTER_AL_ERROR true


namespace spehs
{
	void debugOpenALErrors(const std::string &_file, int _line)
	{
		ALenum errorCode = alGetError();

		switch (errorCode)
		{
		case AL_NO_ERROR:
			//Everything is fine.
			break;

		case AL_INVALID_NAME:
			std::cout << "\nAL_INVALID_NAME!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case AL_INVALID_OPERATION:
			std::cout << "\nAL_INVALID_OPERATION!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case AL_INVALID_ENUM:
			std::cout << "\nAL_INVALID_ENUM!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case AL_INVALID_VALUE:
			std::cout << "\nAL_INVALID_VALUE!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case AL_OUT_OF_MEMORY:
			std::cout << "\nAL_OUT_OF_MEMORY!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		default:
			//This should never happen, maybe...
			std::cout << "\nUndefined OpenAL Error!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;
		}
	}

	void debugOpenALCError(ALCdevice* _device, const std::string &_file, int _line)
	{
		ALCenum errorCode = alcGetError(_device);

		switch (errorCode)
		{
		case ALC_NO_ERROR:
			//Everything is fine.
			break;

		case ALC_INVALID_DEVICE:
			std::cout << "\nALC_INVALID_DEVICE!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case ALC_INVALID_CONTEXT:
			std::cout << "\nALC_INVALID_CONTEXT!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case ALC_INVALID_ENUM:
			std::cout << "\nALC_INVALID_ENUM!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case ALC_INVALID_VALUE:
			std::cout << "\nALC_INVALID_VALUE!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case ALC_OUT_OF_MEMORY:
			std::cout << "\nALC_OUT_OF_MEMORY!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		default:
			//This should never happen, maybe...
			std::cout << "\nUndefined OpenAL ALC Error!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_AL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;
		}
	}
}
#endif