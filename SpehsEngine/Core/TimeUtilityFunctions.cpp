#include "stdafx.h"
#include "SpehsEngine/Core/TimeUtilityFunctions.h"

#include <chrono>
#include <ctime>


namespace se
{
	inline time_t toStdTime(const TimeValueType& timeValue)
	{
		const std::chrono::high_resolution_clock::duration highResolutionDuration(timeValue);
		const std::chrono::system_clock::duration lowResoluationDuration = std::chrono::duration_cast<std::chrono::system_clock::duration>(highResolutionDuration);
		const std::chrono::system_clock::time_point timePoint(lowResoluationDuration);
		return std::chrono::system_clock::to_time_t(timePoint);
	}

	time_t toStdTime(const Time& time)
	{
		return toStdTime(time.value);
	}

	time_t toStdTime(const UtcTime& utcTime)
	{
		return toStdTime(utcTime.value);
	}

	inline TimeValueType toTimeValue(const time_t& t)
	{
		const std::chrono::system_clock::time_point timePoint = std::chrono::system_clock::from_time_t(t);
		const std::chrono::system_clock::duration lowResoluationDuration = timePoint.time_since_epoch();
		const std::chrono::high_resolution_clock::duration highResolutionDuration = std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(lowResoluationDuration);
		return TimeValueType(highResolutionDuration.count());
	}

	Time toTime(const time_t& t)
	{
		return toTimeValue(t);
	}

	UtcTime toUtcTime(const time_t& t)
	{
		return toTimeValue(t);
	}

	TimeInfo tmToTimeInfo(const tm tmp)
	{
		TimeInfo timeInfo;
		timeInfo.second = tmp.tm_sec;
		timeInfo.minute = tmp.tm_min;
		timeInfo.hour = tmp.tm_hour;
		timeInfo.month = tmp.tm_mon;
		timeInfo.year = tmp.tm_year + 1900;
		timeInfo.yearDay = tmp.tm_yday;
		timeInfo.monthDay = tmp.tm_mday - 1;
		timeInfo.weekDay = tmp.tm_wday;
		if (tmp.tm_isdst > 0)
		{
			timeInfo.dayligtSavingsFlag.emplace(true);
		}
		else if (tmp.tm_isdst == 0)
		{
			timeInfo.dayligtSavingsFlag.emplace(false);
		}
		return timeInfo;
	}

	std::optional<TimeInfo> getTimeInfo(const Time& time)
	{
		const time_t t = toStdTime(time);
		tm tmp;
		errno_t error = localtime_s(&tmp, &t);
		if (error == 0)
		{
			return tmToTimeInfo(tmp);
		}
		else
		{
			return std::nullopt;
		}
	}

	std::optional<TimeInfo> getTimeInfo(const UtcTime& utcTime)
	{
		time_t t = toStdTime(utcTime);
		tm tmp;
		errno_t error = gmtime_s(&tmp, &t);
		if (error == 0)
		{
			return tmToTimeInfo(tmp);
		}
		else
		{
			return std::nullopt;
		}
	}

	std::string getTimeInfoDebugString(const Time& time)
	{
		if (const std::optional<TimeInfo> timeInfo = getTimeInfo(time))
		{
			return timeInfo->toString() + " [local]";
		}
		else
		{
			return getDurationDisplayString(time);
		}
	}

	std::string getTimeInfoDebugString(const UtcTime& time)
	{
		if (const std::optional<TimeInfo> timeInfo = getTimeInfo(time))
		{
			return timeInfo->toString() + " [UTC]";
		}
		else
		{
			return getDurationDisplayString(Time(time.value));
		}
	}

	tm getTm(const TimeInfo& timeInfo)
	{
		tm tmp;
		tmp.tm_year = timeInfo.year - 1900;
		tmp.tm_mon = timeInfo.month;
		tmp.tm_sec = timeInfo.second;
		tmp.tm_min = timeInfo.minute;
		tmp.tm_hour = timeInfo.hour;
		tmp.tm_mon = timeInfo.month;
		tmp.tm_year = timeInfo.year - 1900;
		tmp.tm_mday = timeInfo.monthDay + 1;
		if (timeInfo.dayligtSavingsFlag)
		{
			if (*timeInfo.dayligtSavingsFlag)
			{
				tmp.tm_isdst = 1;
			}
			else
			{
				tmp.tm_isdst = 0;
			}
		}
		else
		{
			tmp.tm_isdst = -1;
		}
		return tmp;
	}

	std::optional<Time> TimeInfo::getTime() const
	{
		tm tmp = getTm(*this);
		time_t t = mktime(&tmp);
		if (t == -1)
		{
			return std::nullopt;
		}
		else
		{
			return toTime(t);
		}
	}

	std::optional<UtcTime> TimeInfo::getUtcTime() const
	{
		tm tmp = getTm(*this);
		time_t t = mktime(&tmp);
		if (t == -1)
		{
			return std::nullopt;
		}
		else
		{
			return toUtcTime(t);
		}
	}

	std::string TimeInfo::toString() const
	{
		return formatString("%i.%i.%i [%i:%i:%i]", monthDay + 1, month + 1, year, hour, minute, second);
	}

	std::string getDurationDisplayString(const Time& time)
	{
		std::string string;
		int seconds = int(time.asSeconds());
		int minutes = seconds / 60;
		int hours = minutes / 60;
		int days = hours / 24;
		seconds -= 60 * minutes;
		minutes -= 60 * hours;
		hours -= 24 * days;
		if (days == 1)
		{
			string += "1 day ";
		}
		else if (days > 1)
		{
			string += std::to_string(days) + " days ";
		}
		if (hours == 1)
		{
			string += "1 hour ";
		}
		else if (hours > 1)
		{
			string += std::to_string(hours) + " hours ";
		}
		if (minutes == 1)
		{
			string += "1 minute ";
		}
		else if (minutes > 1)
		{
			string += std::to_string(minutes) + " minutes ";
		}
		if (seconds == 1)
		{
			string += "1 second";
		}
		else
		{
			string += std::to_string(seconds) + " seconds";
		}
		return string;
	}

	std::optional<UtcTime> localToUTC(const Time& localTime)
	{
		if (const std::optional<TimeInfo> timeInfo = getTimeInfo(localTime))
		{
			if (const std::optional<UtcTime> utcTime = timeInfo->getUtcTime())
			{
				return *utcTime;
			}
		}
		return std::nullopt;
	}

	std::optional<Time> utcToLocal(const UtcTime& utcTime)
	{
		if (const std::optional<TimeInfo> timeInfo = getTimeInfo(utcTime))
		{
			if (const std::optional<Time> time = timeInfo->getTime())
			{
				return *time;
			}
		}
		return std::nullopt;
	}
}
