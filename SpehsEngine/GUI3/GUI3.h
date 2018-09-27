#pragma once
#include <string>

namespace se
{
	class InputLib;
	class AudioLib;
	class GUI3Lib
	{
	public:
		/* Initializes gui3 library */
		GUI3Lib(const InputLib& inputLib, const AudioLib& audioLib);

		/* Uninitializes gui3 library */
		~GUI3Lib();

		/* Returns whether the gui library has successfully been initialized */
		static bool isValid();

		/* Returns gui library version in a readable string format */
		static std::string getVersion();
	};
}