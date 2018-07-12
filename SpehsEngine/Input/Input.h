#pragma once
#include <string>

namespace se
{
	class RenderingLib;
	class InputLib
	{
	public:
		/* Initializes input library */
		InputLib(const RenderingLib& renderingLib);

		/* Uninitializes input library */
		~InputLib();

		/* Returns whether the input library has successfully been initialized */
		static bool isValid();

		/* Returns engine input libary version in a readable string format */
		static std::string getVersion();
	};
}