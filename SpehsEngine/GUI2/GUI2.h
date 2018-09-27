#pragma once
#include <string>

namespace se
{
	class InputLib;
	class AudioLib;
	class GUI2Lib
	{
	public:
		/* Initializes gui2 library */
		GUI2Lib(const InputLib& inputLib, const AudioLib& audioLib);

		/* Uninitializes gui2 library */
		~GUI2Lib();

		/* Returns whether the gui library has successfully been initialized */
		static bool isValid();

		/* Returns gui library version in a readable string format */
		static std::string getVersion();
	};
}