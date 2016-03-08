#include "Exceptions.h"

#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>


namespace spehs
{
	namespace exceptions
	{
		void fatalError(std::string errorString)
		{
			std::cout << std::endl;
			std::cout << errorString << std::endl;
			std::cout << "Press 'Enter' to quit." << std::endl;
			std::cin.ignore();
			SDL_Quit();
			exit(1);
		}
		void unexpectedError(std::string errorString)
		{
			std::cout << std::endl;
			std::cout << "Unexpected error: " << errorString << std::endl;
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
		}
		void warning(std::string warningString)
		{
			std::cout << std::endl;
			std::cout << "Warning: " << warningString << std::endl;
		}
	}
}