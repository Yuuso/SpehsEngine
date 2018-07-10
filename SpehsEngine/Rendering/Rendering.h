#pragma once
#include <string>

namespace spehs
{
	class MathLib;
	class RenderingLib
	{
	public:
		/* Initializes rendering library */
		RenderingLib(const MathLib& mathLib);

		/* Uninitializes rendering library */
		~RenderingLib();

		/* Returns whether the rendering library has successfully been initialized */
		static bool isValid();

		/* Returns rendering library version in a readable string format */
		static std::string getVersion();
	};
}