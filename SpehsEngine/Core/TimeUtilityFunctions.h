#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/UtcTime.h"
#include "SpehsEngine/Core/Serial/Serial.h"
#include <string>


namespace se
{
	namespace time
	{
		class Time;

		struct TimeInfo
		{
			// Note that only the value for 'monthDay' is used - 'weekDay' and 'yearDay' are ignored.
			std::optional<Time> getTime() const;
			std::optional<UtcTime> getUtcTime() const;
			std::string toString() const;

			int second = 0;		// Seconds after the minute - [0, 60] including leap second
			int minute = 0;		// Minutes after the hour - [0, 59]
			int hour = 0;		// Hours since midnight - [0, 23]
			int month = 0;		// Months since January - [0, 11]
			int year = 0;		// Current year
			int yearDay = 0;	// Days since January 1 - [0, 365]
			int monthDay = 0;	// Day of the month - [0, 30]
			int weekDay = 0;	// Days since Sunday - [0, 6]
			std::optional<bool> dayligtSavingsFlag; // Set when known
		};

		std::optional<TimeInfo> getTimeInfo(const Time& time);
		std::optional<TimeInfo> getTimeInfo(const UtcTime& time);

		// Simplify dev code by omitting getTimeInfo()'s optional checking and instead printing something if the optional value is not set
		std::string getTimeInfoDebugString(const Time& time);
		std::string getTimeInfoDebugString(const UtcTime& time);

		// Format: '(X days) (Y hours) (Z minutes) W seconds'
		std::string getDurationDisplayString(const Time& time);

		// Returns local point of time in UTC time
		std::optional<UtcTime> localToUTC(const Time& localTime);
		std::optional<Time> utcToLocal(const UtcTime& utcTime);
	}

	template<> template<typename S, typename T>
	static bool se::Serial<time::Time>::serial(S& _serial, T _time)
	{
		se_serial(_serial, _time.value, "t");
		return true;
	}
}