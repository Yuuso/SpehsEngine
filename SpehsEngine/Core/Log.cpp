
#include "stdafx.h"
#include "Log.h"

#if !defined(SE_FINAL_RELEASE)
#include "SystemMessageBox.h"
#endif

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <iostream>
#include <mutex>


namespace se
{
	namespace log
	{
		std::mutex mutex;
		static void writeConsole(const std::string_view message, [[maybe_unused]] const TextColor color)
		{
#if defined(_WIN32)
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
#if defined(_WIN32)
			if (color != NONE)
			{
				BOOL result;
				result = SetConsoleTextAttribute(handle, oldColorAttributes);
				se_assert(result != FALSE);
			}
#endif
		}

		void info([[maybe_unused]] const std::string_view message, [[maybe_unused]] const TextColor color)
		{
#if !defined(SE_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(message, color);
#endif
		}
		void info([[maybe_unused]] const std::string_view message)
		{
#if !defined(SE_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(message, NONE);
#endif
		}
		void warning([[maybe_unused]] const std::string_view message)
		{
#if !defined(SE_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			std::string warningMessage("Warning: ");
			warningMessage += message;
			writeConsole(warningMessage, YELLOW);
#endif
		}
		void error(const std::string_view message)
		{
#if false//!defined(SE_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			std::string errorMessage("Error: ");
			errorMessage += message;
			writeConsole(errorMessage, RED);
			writeConsole("Press enter to continue...", NONE);
			_ASSERT(false && message.c_str()); // NOTE: use the non se_assert here!
			std::getchar();
#else
			const std::string fatalError(message);
			SystemMessageBox("Fatal Error!", fatalError, BUTTONS_OK | ICON_ERROR);
			exit(1);
#endif
		}
	}
}