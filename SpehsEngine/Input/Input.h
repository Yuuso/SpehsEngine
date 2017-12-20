#pragma once
#include <string>

namespace spehs
{
	namespace input
	{
		/* Initializes input library */
		int initialize();

		/* Uninitializes input library */
		void uninitialize();

		/* Returns whether the input library has successfully been initialized */
		bool isInitialized();

		/* Returns engine input libary version in a readable string format */
		std::string getVersion();

		class Window;
		/* Get a pointer to the main window */
		Window* getMainWindow();
	}
}