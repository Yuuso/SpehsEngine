
#include "stdafx.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Time.h"

#if !defined(SE_FINAL_RELEASE)
#include "SpehsEngine/Core/SystemMessageBox.h"
#endif

#if defined(_WIN32)
#include <Windows.h>
#endif

#include <iostream>
#include <iomanip>
#include <mutex>


namespace se
{
	namespace log
	{
		static std::mutex mutex;
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
		static void writeConsole(const std::string_view message, [[maybe_unused]] const TextColor color)
		{
#if defined(_WIN32)
			HANDLE handle = INVALID_HANDLE_VALUE;
			WORD oldColorAttributes = 0;
			{
				handle = GetStdHandle(STD_OUTPUT_HANDLE);
				se_assert(handle != INVALID_HANDLE_VALUE);
				CONSOLE_SCREEN_BUFFER_INFO info;
				BOOL result;
				result = GetConsoleScreenBufferInfo(handle, &info);
				se_assert(result != FALSE);
				oldColorAttributes = info.wAttributes;
			}
			{
				BOOL result;
				result = SetConsoleTextAttribute(handle, (WORD)TextColor::DARKGRAY);
				se_assert(result != FALSE);
			}
#endif
			const time::Time timeNow = time::getRunTime();
			const int minutes = static_cast<int>(timeNow.asSeconds()) / 60;
			const int seconds = static_cast<int>(timeNow.asSeconds()) % 60;
			const int milliseconds = static_cast<int>(timeNow.asMilliseconds()) % 1000;
			std::cout << std::setfill('0') << std::setw(2) << minutes << ":" << std::setw(2) << seconds << "." << std::setw(3) << milliseconds;
			std::cout << "  ";
#if defined(_WIN32)
			if (color != NONE)
			{
				BOOL result;
				result = SetConsoleTextAttribute(handle, (WORD)color);
				se_assert(result != FALSE);
			}
			else
			{
				BOOL result;
				result = SetConsoleTextAttribute(handle, oldColorAttributes);
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
#endif

		void info([[maybe_unused]] const std::string_view message, [[maybe_unused]] const TextColor color)
		{
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(message, color);
#endif
		}
		void info([[maybe_unused]] const std::string_view message)
		{
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(message, NONE);
#endif
		}
		void warning([[maybe_unused]] const std::string_view message)
		{
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			std::string warningMessage("Warning: ");
			warningMessage += message;
			writeConsole(warningMessage, YELLOW);
#endif
		}
		void error(const std::string_view message)
		{
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			std::string errorMessage("Error: ");
			errorMessage += message;
			writeConsole(errorMessage, RED);
			SystemMessageBox("Fatal Error!", errorMessage, BUTTONS_OK | ICON_ERROR);
			//writeConsole("Press enter to continue...", NONE);
			//std::getchar();
#else
			const std::string fatalError(message);
			SystemMessageBox("Fatal Error!", fatalError, BUTTONS_OK | ICON_ERROR);
			exit(1);
#endif
		}
		void debug([[maybe_unused]] const std::string& _message)
		{
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			if (!_message.empty())
			{
				OutputDebugString(_message.c_str());
				if (_message.back() != '\n')
					OutputDebugString("\n");
			}
#endif
		}
	}
}