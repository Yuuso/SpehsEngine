#pragma once


namespace se
{
	class DebugLib
	{
	public:
		/* Initializes debug library */
		DebugLib();

		/* Uninitializes debug library */
		~DebugLib();

		/* Returns whether the debug library has successfully been initialized */
		static bool isValid();

		/* Returns debug library version in a readable string format */
		static std::string getVersion();
	};
}