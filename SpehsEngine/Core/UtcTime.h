#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include <optional>
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

			Time getTime() const { return Time(value); }
			Time timeSince(const UtcTime& other) const;

			void operator=(const UtcTime& other);
			void operator+=(const UtcTime& other);
			void operator-=(const UtcTime& other);
			UtcTime operator+(const UtcTime& other) const;
			UtcTime operator-(const UtcTime& other) const;
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

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& writeBuffer, const time::UtcTime& utcTime);
	bool readFromBuffer(ReadBuffer& readBuffer, time::UtcTime& utcTime);
}