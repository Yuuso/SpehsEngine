#pragma once
#include <string>

namespace spehs
{
	class CoreLib;
	class RenderingLib
	{
	public:
		/* Initializes rendering library */
		RenderingLib(const CoreLib& coreLib);

		/* Uninitializes rendering library */
		~RenderingLib();

		/* Returns whether the rendering library has successfully been initialized */
		static bool isValid();

		/* Returns rendering library version in a readable string format */
		static std::string getVersion();
	};
}