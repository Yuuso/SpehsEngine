#pragma once
#include <string>

namespace spehs
{
	class CoreLib
	{
	public:
		/* Initializes core library */
		CoreLib();

		/* Uninitializes core library */
		~CoreLib();

		/* Returns whether the core library has successfully been initialized */
		static bool isValid();
		
		/* Returns core library version in a readable string format */
		static std::string getVersion();
	};
}