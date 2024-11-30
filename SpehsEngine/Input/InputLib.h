#pragma once


namespace se
{
	class InputLib
	{
	public:
		/* Initializes input library */
		InputLib();

		/* Uninitializes input library */
		~InputLib();

		/* Returns whether the input library has successfully been initialized */
		static bool isValid();

		/* Returns engine input libary version in a readable string format */
		static std::string getVersion();
	};
}