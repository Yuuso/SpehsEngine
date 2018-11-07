#include "stdafx.h"
#include "SpehsEngine/Core/Log.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <iostream>
#include <mutex>

namespace se
{
	namespace log
	{
		std::mutex mutex;
		static void writeConsole(const std::string& message, /*[[maybe_unused]]*/ const TextColor color)
		{
#ifdef _WIN32
			HANDLE handle = INVALID_HANDLE_VALUE;
			WORD oldColorAttributes = 0;
			if (color != NONE)
			{
				handle = GetStdHandle(STD_OUTPUT_HANDLE);
				se_assert(handle != INVALID_HANDLE_VALUE);
				CONSOLE_SCREEN_BUFFER_INFO info;
				BOOL result;
				result = GetConsoleScreenBufferInfo(handle, &info);
				se_assert(result != FALSE);
				oldColorAttributes = info.wAttributes;
				result = SetConsoleTextAttribute(handle, (WORD)color);
				se_assert(result != FALSE);
			}
#endif
			std::cout << message << std::endl;
#ifdef _WIN32
			if (color != NONE)
			{
				BOOL result;
				result = SetConsoleTextAttribute(handle, oldColorAttributes);
				se_assert(result != FALSE);
			}
#else
		(void)color;
#endif
		}

		void info(const std::string& message, const TextColor color)
		{
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(message, color);
		}
		void info(const std::string& message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(message, NONE);
		}
		void warning(const std::string& message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole("Warning: " + message, YELLOW);
		}
		void error(const std::string& message)
		{
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole("Error: " + message, RED);
			writeConsole("Press enter to continue...", NONE);
			_ASSERT(false && message.c_str()); // NOTE: use the non se_assert here!
			std::getchar();
		}
	}
}