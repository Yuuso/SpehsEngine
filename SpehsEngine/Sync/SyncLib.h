#pragma once
#include <string>

namespace se
{
	class NetLib;
	class SyncLib
	{
	public:
		/* Initializes sync library */
		SyncLib(const NetLib& netLib);

		/* Uninitializes sync library */
		~SyncLib();

		/* Returns whether the sync library has successfully been initialized */
		static bool isValid();

		/* Returns engine sync libary version in a readable string format */
		static std::string getVersion();
	};
}