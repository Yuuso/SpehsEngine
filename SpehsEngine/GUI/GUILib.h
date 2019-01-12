#pragma once
#include <string>

namespace se
{
	class InputLib;
	class AudioLib;
	class GUILib
	{
	public:
		/* Initializes gui library */
		GUILib(const InputLib& inputLib, const AudioLib& audioLib);

		/* Uninitializes gui library */
		~GUILib();

		/* Returns whether the gui library has successfully been initialized */
		static bool isValid();

		/* Returns gui library version in a readable string format */
		static std::string getVersion();
	};
}