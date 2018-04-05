#pragma once
#include <string>

namespace spehs
{
	class SyncLib;
	class GPIOLib
	{
	public:
		/* Initializes gpio library */
		GPIOLib(const SyncLib& syncLib);

		/* Uninitializes gpio library */
		~GPIOLib();

		/* Returns whether the gpio library has successfully been initialized */
		static bool isValid();

		/* Returns engine gpio libary version in a readable string format */
		static std::string getVersion();
	};
}