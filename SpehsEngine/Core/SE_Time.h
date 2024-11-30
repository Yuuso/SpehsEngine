#pragma once

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
			constexpr TimeValueType second = 1000000000;
			constexpr TimeValueType millisecond = second / 1000;
			constexpr TimeValueType microsecond = millisecond / 1000;
			constexpr TimeValueType nanosecond = microsecond / 1000;
		}

		class Time
		{
		public:

			constexpr Time();
			constexpr Time(const TimeValueType _value);
			Time(const Time& other);

			static const Time zero;

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
			Time operator-() const;
			void operator=(const Time& other);
			bool operator==(const Time& other) const;
			bool operator!=(const Time& other) const;
			bool operator>(const Time& other) const;
			bool operator<(const Time& other) const;
			bool operator>=(const Time& other) const;
			bool operator<=(const Time& other) const;

			template<typename T = float> inline constexpr T asSeconds() const		{ return (T)value / (T)conversionRate::second;		}
			template<typename T = float> inline constexpr T asMilliseconds() const	{ return (T)value / (T)conversionRate::millisecond; }
			template<typename T = float> inline constexpr T asMicroseconds() const	{ return (T)value / (T)conversionRate::microsecond; }
			template<typename T = float> inline constexpr T asNanoseconds() const	{ return (T)value / (T)conversionRate::nanosecond;	}

			TimeValueType value;
		};

		constexpr Time::Time()
			: value(0) { }
		constexpr Time::Time(const TimeValueType _value)
			: value(_value) { }

		Time operator*(const float multiplier, const Time time);
		Time operator*(const int multiplier, const Time time);

		template<typename T = float> inline T asSeconds(const Time time)		{ return time.asSeconds<T>();		}
		template<typename T = float> inline T asMilliseconds(const Time time)	{ return time.asMilliseconds<T>();	}
		template<typename T = float> inline T asMicroseconds(const Time time)	{ return time.asMicroseconds<T>();	}
		template<typename T = float> inline T asNanoseconds(const Time time)	{ return time.asNanoseconds<T>();	}

		inline constexpr Time fromSeconds(const double seconds)				{ return TimeValueType(seconds * static_cast<double>(conversionRate::second)); }
		inline constexpr Time fromMilliseconds(const double milliseconds)	{ return TimeValueType(milliseconds * static_cast<double>(conversionRate::millisecond)); }
		inline constexpr Time fromMicroseconds(const double microseconds)	{ return TimeValueType(microseconds * static_cast<double>(conversionRate::microsecond)); }
		inline constexpr Time fromNanoseconds(const double nanoseconds)		{ return TimeValueType(nanoseconds * static_cast<double>(conversionRate::nanosecond)); }

		/* Initializes the time system. */
		void initialize();

		/* Puts thread to sleep for (at least) this much time. */
		void sleep(const Time time);

		/* Waits in a while loop until specified time has passed. More precise than sleep, but hogs processor for the duration of delay. */
		void delay(const Time time);

		/* Get date and time string in strftime format. */
		std::string date(const std::string& format);

		// Return time since the given time stamp
		Time timeSince(const Time _time);

		/* Returns the current time stamp, relative to 'some' context. See getRunTime() for an alternative. */
		Time now();

		/* Returns system time. System time is not as precise as now(). Note that the user is able to change system time. */
		Time getSystemTime();

		/* Returns current time since time was initialized. Less efficient than using now(). */
		Time getRunTime();

		/* Returns time when se::time was initialized. */
		Time getInitializationTime();

		/*
			Potentially more efficient than now().
			Prefer now() for non-profiling purposes.
		*/
		Time getProfilerTimestamp();
	}
}