#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include <string>


namespace se
{
	namespace time
	{
		// Stronger type for representing an UTC timestamp
		struct UtcTime
		{
			UtcTime() = default;
			UtcTime(const TimeValueType _value) : value(_value) {}

			Time timeSince(const UtcTime& other) const;

			void operator=(const UtcTime& other);
			void operator+=(const Time& other);
			void operator-=(const Time& other);
			UtcTime operator+(const Time& other) const;
			UtcTime operator-(const Time& other) const;
			bool operator==(const UtcTime& other) const;
			bool operator!=(const UtcTime& other) const;
			bool operator>(const UtcTime& other) const;
			bool operator<(const UtcTime& other) const;
			bool operator>=(const UtcTime& other) const;
			bool operator<=(const UtcTime& other) const;

			TimeValueType value = 0;
		};

		UtcTime getUtcTime();
	}
}

template<> template<typename S, typename T>
static bool se::Serial<se::time::UtcTime>::serial(S& _serial, T _value)
{
	se_serial(_serial, _value.value, "value");
	return true;
}