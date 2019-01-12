#pragma once
#include <string>

namespace se
{
	class MathLib;
	class PhysicsLib
	{
	public:
		/* Initializes physics library */
		PhysicsLib(const MathLib& mathLib);

		/* Uninitializes physics library */
		~PhysicsLib();

		/* Returns whether the physics library has successfully been initialized */
		static bool isValid();
		
		/* Returns physics library version in a readable string format */
		static std::string getVersion();
	};
}