#pragma once
#include <string>

namespace spehs
{
	class ApplicationData;
	///Initializes rebuut. If a pointer to a custom instance of application data is given, application data is read and set there.
	int initialize(std::string _windowName, ApplicationData* customApplicationDataInstance = nullptr);
	void uninitialize();
	std::string getEngineVersion();
}