#pragma once
#include <string>

namespace spehs
{
	class CoreLib;
	class AudioLib
	{
	public:
		/* Initializes audio library */
		AudioLib(const CoreLib& coreLib);

		/* Uninitializes audio library */
		~AudioLib();

		/* Returns whether the audio library has successfully been initialized */
		static bool isValid();

		/* Returns audio library version in a readable string format */
		static std::string getVersion();
	};
}