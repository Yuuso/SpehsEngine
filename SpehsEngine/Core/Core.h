#pragma once
#include <string>

namespace spehs
{
	namespace core
	{
		/* Initializes core library */
		int initialize();

		/* Uninitializes core library */
		void uninitialize();

		/* Returns whether the core library has successfully been initialized */
		bool isInitialized();
		
		/* Returns core library version in a readable string format */
		std::string getVersion();
	}
}