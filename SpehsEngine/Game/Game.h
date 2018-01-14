#pragma once
#include <string>

namespace spehs
{
	class InputLib;
	class AudioLib;
	class GameLib
	{
		/* Initializes game library */
		GameLib(const InputLib& inputLib, const AudioLib& audioLib);

		/* Uninitializes game library */
		~GameLib();

		/* Returns whether the game library has successfully been initialized */
		static bool isValid();

		/* Returns game library version in a readable string format */
		static std::string getVersion();
	};
}