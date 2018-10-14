#include "stdafx.h"
#include <iostream>
#include <mutex>
#include "SpehsEngine/Core/Log.h"

namespace se
{
	namespace log
	{
		std::mutex mutex;
		void info(const std::string& message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << message << std::endl;
		}
		void warning(const std::string& message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "Warning: " << message << std::endl;
		}
		void error(const std::string& message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "Error: " << message << "\nPress enter to continue..." << std::endl;
			_ASSERT(false && message.c_str());//NOTE: use the non se_assert here!
			std::getchar();
		}
		void info(const char* message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << message << std::endl;
		}
		void warning(const char* message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "Warning: " << message << std::endl;
		}
		void error(const char* message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::cout << "Error: " << message << "\nPress enter to continue..." << std::endl;
			_ASSERT(false && message);//NOTE: use the non se_assert here!
			std::getchar();
		}
	}
}