#pragma once
#include <string>

namespace spehs
{
	namespace game
	{
		/* Initializes game library */
		int initialize();

		/* Uninitializes game library */
		void uninitialize();

		/* Returns whether the game library has successfully been initialized */
		bool isInitialized();

		/* Returns game library version in a readable string format */
		std::string getVersion();
	}
}