#include "stdafx.h"
#include "SpehsEngine/Core/SE_Time.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/TimeUtilityFunctions.h"
#include <chrono>
#include <sstream>
#include <iomanip>
#include <thread>
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
#include <Windows.h>
#endif


namespace se
{
	namespace time
	{
		static_assert(conversionRate::second == std::chrono::high_resolution_clock::time_point::period::den, "TODO");
		static_assert(conversionRate::second == std::chrono::system_clock::time_point::period::den * 100, "TODO");

		namespace
		{
			bool initialized = false;
			std::chrono::high_resolution_clock::rep initializationTime;

#if SE_PLATFORM == SE_PLATFORM_WINDOWS
			double queryPerformanceFrequency = 0.0f;
#endif
		}

		void initialize()
		{
			if (initialized)
			{
				log::warning("se::time has already been initialized!");
				return;
			}

#if SE_PLATFORM == SE_PLATFORM_WINDOWS
			{
				LARGE_INTEGER counter;
				if (!QueryPerformanceFrequency(&counter))
				{
					log::error("QueryPerformanceFrequency failed!");
				}
				queryPerformanceFrequency = double(counter.QuadPart);
			}
#endif

			initializationTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			initialized = true;
			log::info("SpehsEngine time accuracy is " + std::to_string(conversionRate::second) + " ticks per second.");
		}

		Time now()
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		}

		Time getSystemTime()
		{
			static const int64_t multiplier = std::chrono::high_resolution_clock::time_point::period::den / std::chrono::system_clock::time_point::period::den;
			return std::chrono::system_clock::now().time_since_epoch().count() * multiplier;
		}

		Time getRunTime()
		{
			se_assert(initialized);
			return std::chrono::high_resolution_clock::now().time_since_epoch().count() - initializationTime;
		}

		Time getInitializationTime()
		{
			se_assert(initialized);
			return initializationTime;
		}

		Time getProfilerTimestamp()
		{
#if SE_PLATFORM == SE_PLATFORM_WINDOWS
			LARGE_INTEGER counter;
			if (QueryPerformanceCounter(&counter))
			{
				return time::Time(time::TimeValueType(double(counter.QuadPart) / (queryPerformanceFrequency / double(time::conversionRate::second))));
			}
			else
			{
				log::error("QueryPerformanceCounter failed.");
			}
#else
#error getProfilerTimestamp() implementation is missing.
#endif
			return now();
		}

		void sleep(const Time time)
		{
			se_assert(time.value >= 0);
			std::chrono::duration<TimeValueType, std::chrono::high_resolution_clock::time_point::period> duration((TimeValueType)time.value);
			std::this_thread::sleep_for(duration);
		}

		void delay(const Time time)
		{
			const Time begin(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			while (true)
			{
				const Time now(std::chrono::high_resolution_clock::now().time_since_epoch().count());
				if (now - begin >= time)
					return;
			}
		}

		Time timeSince(const Time _time)
		{
			return now() - _time;
		}

		std::string date(const std::string& format)
		{
			std::time_t rawtime = std::time(nullptr);
			std::tm tm;
			const errno_t result = localtime_s(&tm, &rawtime);
			se_assert(result == 0);

			std::ostringstream stringstream;
			stringstream << std::put_time(&tm, format.c_str());
			return stringstream.str();
		}

		std::string engineBuildYear()
		{
			std::string year;
			year.push_back(__DATE__[7]);
			year.push_back(__DATE__[8]);
			year.push_back(__DATE__[9]);
			year.push_back(__DATE__[10]);
			return year;
		}
		std::string engineBuildMonth()
		{
			return __DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? "01" : "06")
				: __DATE__[2] == 'b' ? "02"
				: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? "03" : "04")
				: __DATE__[2] == 'y' ? "05"
				: __DATE__[2] == 'l' ? "07"
				: __DATE__[2] == 'g' ? "08"
				: __DATE__[2] == 'p' ? "09"
				: __DATE__[2] == 't' ? "10"
				: __DATE__[2] == 'v' ? "11"
				: "12";
		}
		std::string engineBuildDay()
		{
			std::string day;
			day.push_back((__DATE__[4] == ' ' ? '0' : __DATE__[4]));
			day.push_back(__DATE__[5]);
			return day;
		}
		std::string engineBuildHour()
		{
			std::string hour;
			hour.push_back(__TIME__[0]);
			hour.push_back(__TIME__[1]);
			return hour;
		}
		std::string engineBuildMinute()
		{
			std::string minute;
			minute.push_back(__TIME__[3]);
			minute.push_back(__TIME__[4]);
			return minute;
		}
		std::string engineBuildSecond()
		{
			std::string second;
			second.push_back(__TIME__[6]);
			second.push_back(__TIME__[7]);
			return second;
		}
	}
}
