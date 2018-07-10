#pragma once
#include <string>

namespace spehs
{
	class CoreLib;
	class MathLib
	{
	public:
		/* Initializes math library */
		MathLib(const CoreLib& coreLib);

		/* Uninitializes math library */
		~MathLib();

		/* Returns whether the math library has successfully been initialized */
		static bool isValid();
		
		/* Returns math library version in a readable string format */
		static std::string getVersion();
	};
}