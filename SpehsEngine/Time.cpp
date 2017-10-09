#include <algorithm>
#include <iostream>
#include <chrono>
#include <mutex>

#include "ApplicationData.h"
#include "BatchManager.h"
#include "Camera2D.h"
#include "Time.h"
#include "Text.h"


namespace spehs
{
	namespace time
	{
		namespace conversionRate
		{
			const TimeValueType second = std::chrono::high_resolution_clock::time_point::period::den;
			const TimeValueType millisecond = second / 1000;
			const TimeValueType microsecond = millisecond / 1000;
			const TimeValueType nanosecond = microsecond / 1000;
		}

		//Global variables
		Time maxDeltaTime(0);
		
		//Local variables
		std::chrono::high_resolution_clock::rep initializationTime;

		void initialize()
		{
			initializationTime = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			std::cout << "SpehsEngine time accuracy is " + std::to_string(conversionRate::second) + " ticks per second.";
		}
		
		Time now()
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch().count();
		}

		Time getRunTime()
		{
			return std::chrono::high_resolution_clock::now().time_since_epoch().count() - initializationTime;
		}

		void delay(const Time& time)
		{
			const Time begin(std::chrono::high_resolution_clock::now().time_since_epoch().count());
			while (true)
			{
				const Time now(std::chrono::high_resolution_clock::now().time_since_epoch().count());
				if (now - begin >= time)
					return;
			}
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

		//TIME STRUCT
#pragma region TIME STRUCT
		const Time Time::zero(0);
		Time::Time()
			: value(0)
		{
		}
		Time::Time(const TimeValueType _value)
			: value(_value)
		{
		}
		Time::Time(const Time& other)
			: value(other.value)
		{
		}
		Time Time::operator*(const float factor) const
		{
			return Time(value * factor);
		}
		Time Time::operator*(const int factor) const
		{
			return Time(value * factor);
		}
		Time Time::operator/(const float factor) const
		{
			return Time(value / factor);
		}
		Time Time::operator/(const int factor) const
		{
			return Time(value / factor);
		}
		Time Time::operator+(const Time& other) const
		{
			return Time(value + other.value);
		}
		Time Time::operator-(const Time& other) const
		{
			return Time(value - other.value);
		}
		void Time::operator=(const Time& other)
		{
			value = other.value;
		}
		void Time::operator*=(const float factor)
		{
			value *= factor;
		}
		void Time::operator*=(const int factor)
		{
			value *= factor;
		}
		void Time::operator/=(const float factor)
		{
			value /= factor;
		}
		void Time::operator/=(const int factor)
		{
			value /= factor;
		}
		void Time::operator+=(const Time& other)
		{
			value += other.value;
		}
		void Time::operator-=(const Time& other)
		{
			value -= other.value;
		}
		bool Time::operator==(const Time& other) const
		{
			return value == other.value;
		}
		bool Time::operator>(const Time& other) const
		{
			return value > other.value;
		}
		bool Time::operator<(const Time& other) const
		{
			return value < other.value;
		}
		bool Time::operator>=(const Time& other) const
		{
			return value >= other.value;
		}
		bool Time::operator<=(const Time& other) const
		{
			return value <= other.value;
		}
#pragma endregion
	}
}