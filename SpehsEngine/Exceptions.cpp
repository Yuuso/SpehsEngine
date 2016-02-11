#include "Exceptions.h"

#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>


namespace SpehsEngine
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
	}
}