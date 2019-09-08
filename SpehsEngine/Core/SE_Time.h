#pragma once
#include <stdint.h>
#include <string>

#ifdef delay
#undef delay
#endif

namespace se
{
	namespace time
	{
		typedef int64_t TimeValueType;
		namespace conversionRate
		{
			extern const TimeValueType second;
			extern const TimeValueType millisecond;
			extern const TimeValueType microsecond;
			extern const TimeValueType nanosecond;
		}

		class Time
		{
		public:
			static const Time zero;

			Time();
			Time(const TimeValueType _value);
			Time(const Time& other);

			void operator*=(const double factor);
			void operator*=(const float factor);
			void operator*=(const int factor);
			void operator*=(const TimeValueType factor);
			void operator/=(const double divisor);
			void operator/=(const float divisor);
			void operator/=(const int divisor);
			void operator/=(const TimeValueType divisor);
			void operator+=(const Time& other);
			void operator-=(const Time& other);
			Time operator*(const double factor) const;
			Time operator*(const float factor) const;
			Time operator*(const int factor) const;
			Time operator*(const TimeValueType factor) const;
			Time operator/(const double divisor) const;
			Time operator/(const float divisor) const;
			Time operator/(const int divisor) const;
			Time operator/(const TimeValueType divisor) const;
			Time operator+(const Time& other) const;
			Time operator-(const Time& other) const;
			void operator=(const Time& other);
			bool operator==(const Time& other) const;
			bool operator!=(const Time& other) const;
			bool operator>(const Time& other) const;
			bool operator<(const Time& other) const;
			bool operator>=(const Time& other) const;
			bool operator<=(const Time& other) const;

			inline float asSeconds() const { return (float)value / (float)conversionRate::second; }
			inline float asMilliseconds() const { return (float)value / (float)conversionRate::millisecond; }
			inline float asMicroseconds() const { return (float)value / (float)conversionRate::microsecond; }
			inline float asNanoseconds() const { return (float)value / (float)conversionRate::nanosecond; }

			TimeValueType value;
		};

		Time operator*(const float multiplier, const Time time);
		Time operator*(const int multiplier, const Time time);

		inline float asSeconds(const Time time) { return (float)time.value / (float)conversionRate::second; }
		inline float asMilliseconds(const Time time) { return (float)time.value / (float)conversionRate::millisecond; }
		inline float asMicroseconds(const Time time) { return (float)time.value / (float)conversionRate::microsecond; }
		inline float asNanoseconds(const Time time) { return (float)time.value / (float)conversionRate::nanosecond; }

		inline Time fromSeconds(const float seconds) { return TimeValueType((double)seconds * (double)conversionRate::second); }
		inline Time fromMilliseconds(const float milliseconds) { return TimeValueType((double)milliseconds * (double)conversionRate::millisecond); }
		inline Time fromMicroseconds(const float microseconds) { return TimeValueType((double)microseconds * (double)conversionRate::microsecond); }
		inline Time fromNanoseconds(const float nanoseconds) { return TimeValueType((double)nanoseconds * (double)conversionRate::nanosecond); }

		/* Initializes the time system. */
		void initialize();

		/* Puts thread to sleep for (at least) this much time. */
		void sleep(const Time time);

		/* Waits in a while loop until specified time has passed. More precise than sleep, but hogs processor for the duration of delay. */
		void delay(const Time time);

		/* Get date and time string in strftime format. */
		std::string date(const std::string& format);

		/* Returns the current time stamp, relative to 'some' context. See getRunTime() for an alternative. */
		Time now();

		/* Returns current time since time was initialized. Less efficient than using now(). */
		Time getRunTime();

		/* Returns time when se::time was initialized. */
		Time getInitializationTime();
	}

	class WriteBuffer;
	class ReadBuffer;
	void writeToBuffer(WriteBuffer& writeBuffer, const time::Time& time);
	bool readFromBuffer(ReadBuffer& readBuffer, time::Time& time);
}