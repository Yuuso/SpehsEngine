#pragma once
#include <string>

namespace spehs
{
	namespace rendering
	{
		/* Initializes rendering library */
		int initialize();

		/* Uninitializes rendering library */
		void uninitialize();

		/* Returns whether the rendering library has successfully been initialized */
		bool isInitialized();

		/* Returns rendering library version in a readable string format */
		std::string getVersion();
	}
}