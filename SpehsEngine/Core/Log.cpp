#include "stdafx.h"
#include "SpehsEngine/Core/Log.h"

#include "SpehsEngine/Core/LogSignal.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/SystemMessageBox.h"
#include <iostream>
#include <iomanip>
#include <mutex>

#if defined(_WIN32)
#include <Windows.h>
#endif


namespace se
{
	namespace log
	{
		static std::recursive_mutex logSignalMutex;
		static boost::signals2::signal<void(const std::string_view, const LogType)> logSignal;
		void logSignalImpl(const std::string_view string, const LogType logType)
		{
			std::lock_guard<std::recursive_mutex> lock(logSignalMutex);
			logSignal(string, logType);
		}

		static std::mutex mutex;
#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
		static void writeConsole(const std::string_view _message, [[maybe_unused]] const TextColor color)
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
			std::cout << _message << std::endl;
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

		void debug([[maybe_unused]] const std::string& _message)
		{
			logSignalImpl(_message, LogType::Debug);
		#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			if (!_message.empty())
			{
				OutputDebugString(_message.c_str());
				if (_message.back() != '\n')
					OutputDebugString("\n");
			}
		#endif
		}

		void info([[maybe_unused]] const std::string_view _message, [[maybe_unused]] const TextColor color)
		{
			logSignalImpl(_message, LogType::Info);
		#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			writeConsole(_message, color);
		#endif
		}

		void warning([[maybe_unused]] const std::string_view _message)
		{
			logSignalImpl(_message, LogType::Warning);
		#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			std::lock_guard<std::mutex> lock(mutex);
			const std::string warningMessage("Warning: " + _message);
			writeConsole(warningMessage, YELLOW);
		#endif
		}

		void error(const std::string_view _message)
		{
			logSignalImpl(_message, LogType::Error);
			std::lock_guard<std::mutex> lock(mutex);
		#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			const std::string errorMessage("Error: " + _message);
			writeConsole(errorMessage, RED);
		#endif
			SystemMessageBox("Error!", std::string(_message), BUTTONS_OK | ICON_ERROR);
		}

		void fatal(const std::string_view _message)
		{
			logSignalImpl(_message, LogType::Fatal);
			std::lock_guard<std::mutex> lock(mutex);
		#if (SE_CONFIGURATION != SE_CONFIGURATION_FINAL_RELEASE)
			const std::string errorMessage("Fatal Error: " + _message);
			writeConsole(errorMessage, RED);
		#endif
			SystemMessageBox("Fatal Error!", std::string(_message), BUTTONS_OK | ICON_ERROR);
			exit(1);
		}

		void connectToLogSignal(boost::signals2::scoped_connection& scopedConnection, const std::function<void(const std::string_view, const LogType)>& callback)
		{
			std::lock_guard<std::recursive_mutex> lock(logSignalMutex);
			scopedConnection = logSignal.connect(callback);
		}
	}
}