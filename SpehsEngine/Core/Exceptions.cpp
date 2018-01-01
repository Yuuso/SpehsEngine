#include "SpehsEngine/Core/Exceptions.h"

#include <cstdlib>
#include <iostream>


namespace spehs
{
	namespace exceptions
	{
		void fatalError(const std::string errorString)
		{
			std::cout << std::endl;
			std::cout << "Fatal Error: " << errorString << std::endl;
			std::cout << "Press 'Enter' to quit." << std::endl;
			std::cin.ignore();
			//SDL_Quit(); TODO!
			exit(1);
		}
		void unexpectedError(const std::string errorString)
		{
			std::cout << std::endl;
			std::cout << "Unexpected Error: " << errorString << std::endl;
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
		}
		void warning(const std::string warningString)
		{
			std::cout << std::endl;
			std::cout << "Warning: " << warningString << std::endl;
		}
	}
}