#pragma once


namespace se
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