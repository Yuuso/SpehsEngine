#pragma once
#include <string>

namespace se
{
	namespace rendering
	{
		class RenderingLib;
	}

	class InputLib
	{
	public:
		/* Initializes input library */
		InputLib(/*const se::rendering::RenderingLib& renderingLib*/);

		/* Uninitializes input library */
		~InputLib();

		/* Updates input library state. */
		void update();

		/* Returns whether the input library has successfully been initialized */
		static bool isValid();

		/* Returns engine input libary version in a readable string format */
		static std::string getVersion();
	};
}