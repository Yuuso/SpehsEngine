#pragma once
#include <string>

namespace spehs
{
	namespace gui
	{
		/* Initializes gui library */
		int initialize();

		/* Uninitializes gui library */
		void uninitialize();

		/* Returns whether the gui library has successfully been initialized */
		bool isInitialized();

		/* Returns gui library version in a readable string format */
		std::string getVersion();
	}
}