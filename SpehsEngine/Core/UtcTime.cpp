#include "stdafx.h"
#include "SpehsEngine/Core/UtcTime.h"

#include "SpehsEngine/Core/TimeUtilityFunctions.h"


namespace se
{
	namespace time
	{
		Time UtcTime::timeSince(const UtcTime& other) const
		{
			return Time(value - other.value);
		}
		UtcTime UtcTime::operator+(const Time& other) const { return UtcTime(value + other.value); }
		UtcTime UtcTime::operator-(const Time& other) const { return UtcTime(value - other.value); }
		void UtcTime::operator=(const UtcTime& other) { value = other.value; }
		void UtcTime::operator+=(const Time& other) { value += other.value; }
		void UtcTime::operator-=(const Time& other) { value -= other.value; }
		bool UtcTime::operator==(const UtcTime& other) const { return value == other.value; }
		bool UtcTime::operator!=(const UtcTime& other) const { return value != other.value; }
		bool UtcTime::operator>(const UtcTime& other) const { return value > other.value; }
		bool UtcTime::operator<(const UtcTime& other) const { return value < other.value; }
		bool UtcTime::operator>=(const UtcTime& other) const { return value >= other.value; }
		bool UtcTime::operator<=(const UtcTime& other) const { return value <= other.value; }

		UtcTime getUtcTime()
		{
			// HACK
			// TODO: replace with std::chrono::utc_clock in c++20
			const Time systemTime = getSystemTime();
			if (const std::optional<TimeInfo> timeInfo = getTimeInfo(systemTime))
			{
				if (const std::optional<UtcTime> utcTime = timeInfo->getUtcTime())
				{
					return *utcTime;
				}
			}
			return UtcTime();
		}
	}
}
