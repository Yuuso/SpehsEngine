#pragma once
#include <string>

namespace spehs
{
	namespace input
	{
		class Window;

		/* Initializes Input library */
		int initialize(const std::string& _windowName);

		/* Uninitializes Input library */
		void uninitialize();

		/* Returns whether the input library has successfully been initialized */
		bool isInitialized();

		/* Returns engine input libary version in a readable string format */
		std::string getVersion();

		/* Get a pointer to the main window */
		Window* getMainWindow();
	}
}