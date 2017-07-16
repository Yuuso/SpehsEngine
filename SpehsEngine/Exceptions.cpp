#include "Exceptions.h"
#include "Console.h"
#include "Color.h"

#include <cstdlib>
#include <iostream>
#include <SDL/SDL.h>


namespace spehs
{
	namespace exceptions
	{
		static bool consoleLoggingEnabled = true;
		void fatalError(const std::string errorString)
		{
			std::cout << std::endl;
			std::cout << "Fatal Error: " << errorString << std::endl;
			std::cout << "Press 'Enter' to quit." << std::endl;
			std::cin.ignore();
			SDL_Quit();
			exit(1);
		}
		void unexpectedError(const std::string errorString)
		{
			if (consoleLoggingEnabled)
				console::log("Unexpected Error: " + errorString, Color(1.0f, 0.0f, 0.0f));
			std::cout << std::endl;
			std::cout << "Unexpected Error: " << errorString << std::endl;
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
		}
		void warning(const std::string warningString)
		{
			if (consoleLoggingEnabled)
				console::log("Warning: " + warningString, Color(0.9f, 0.6f, 0.1f));
			std::cout << std::endl;
			std::cout << "Warning: " << warningString << std::endl;
		}
	}
}