#pragma once
#include <string>

namespace se
{
	class CoreLib;
	class NetLib
	{
	public:
		/* Initializes net library */
		NetLib(const CoreLib& coreLib);

		/* Uninitializes net library */
		~NetLib();

		/* Returns whether the net library has successfully been initialized */
		static bool isValid();

		/* Returns engine net libary version in a readable string format */
		static std::string getVersion();
	};
}