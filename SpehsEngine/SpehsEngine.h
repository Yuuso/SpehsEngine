#pragma once
#include <string>

namespace spehs
{
	class Window;

	/* Initializes Spehs Engine */
	int initialize(const std::string& _windowName);

	/* Uninitializes Spehs Engine */
	void uninitialize();

	/* Get a pointer to the main window */
	spehs::Window* getMainWindow();

	/* Returns engine version in a readable string format */
	std::string getEngineVersion();
}