#pragma once
#include <string>

namespace spehs
{
	namespace audio
	{
		/* Initializes audio library */
		int initialize();

		/* Uninitializes audio library */
		void uninitialize();

		/* Returns whether the audio library has successfully been initialized */
		bool isInitialized();

		/* Returns audio library version in a readable string format */
		std::string getVersion();
	}
}