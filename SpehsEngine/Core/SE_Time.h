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

			static const Time zero;

			constexpr Time() : value(0) {}
			constexpr Time(const TimeValueType _value) : value(_value) {}
			constexpr Time(const Time& _other) : value(_other.value) {}
			
			constexpr Time operator-() const								{ return Time(-value); }
			constexpr Time operator*(const double _factor) const			{ return Time(TimeValueType((double)value * _factor)); }
			constexpr Time operator*(const float _factor) const				{ return Time(TimeValueType((float)value * _factor)); }
			constexpr Time operator*(const int _factor) const				{ return Time(value * (TimeValueType)_factor); }
			constexpr Time operator*(const TimeValueType _factor) const		{ return Time(value * _factor); }
			constexpr Time operator/(const double _factor) const			{ return Time(TimeValueType((double)value / _factor)); }
			constexpr Time operator/(const float _factor) const				{ return Time(TimeValueType((float)value / _factor)); }
			constexpr Time operator/(const int _factor) const				{ return Time(value / (TimeValueType)_factor); }
			constexpr Time operator/(const TimeValueType _factor) const		{ return Time(value / _factor); }
			constexpr Time operator+(const Time& _other) const				{ return Time(value + _other.value); }
			constexpr Time operator-(const Time& _other) const				{ return Time(value - _other.value); }
			constexpr void operator=(const Time& _other)					{ value = _other.value; }
			constexpr void operator*=(const double _factor)					{ value = TimeValueType((double)value * _factor); }
			constexpr void operator*=(const float _factor)					{ value = TimeValueType((float)value * _factor); }
			constexpr void operator*=(const int _factor)					{ value *= (TimeValueType)_factor; }
			constexpr void operator*=(const TimeValueType _factor)			{ value *= _factor; }
			constexpr void operator/=(const double divisor)					{ value = TimeValueType((double)value / divisor); }
			constexpr void operator/=(const float divisor)					{ value = TimeValueType((float)value / divisor); }
			constexpr void operator/=(const int divisor)					{ value /= (TimeValueType)divisor; }
			constexpr void operator/=(const TimeValueType divisor)			{ value /= divisor; }
			constexpr void operator+=(const Time& _other)					{ value += _other.value; }
			constexpr void operator-=(const Time& _other)					{ value -= _other.value; }
			constexpr bool operator==(const Time& _other) const				{ return value == _other.value; }
			constexpr bool operator!=(const Time& _other) const				{ return value != _other.value; }
			constexpr bool operator>(const Time& _other) const				{ return value > _other.value; }
			constexpr bool operator<(const Time& _other) const				{ return value < _other.value; }
			constexpr bool operator>=(const Time& _other) const				{ return value >= _other.value; }
			constexpr bool operator<=(const Time& _other) const				{ return value <= _other.value; }

			template<typename T = float> constexpr T asSeconds() const		{ return (T)value / (T)conversionRate::second; }
			template<typename T = float> constexpr T asMilliseconds() const	{ return (T)value / (T)conversionRate::millisecond; }
			template<typename T = float> constexpr T asMicroseconds() const	{ return (T)value / (T)conversionRate::microsecond; }
			template<typename T = float> constexpr T asNanoseconds() const	{ return (T)value / (T)conversionRate::nanosecond;	}

			TimeValueType value;
		};

		inline constexpr const Time Time::zero = Time(0);

		constexpr Time operator*(const double _multiplier, const Time _time)		{ return Time(TimeValueType((double)_time.value * _multiplier)); }
		constexpr Time operator*(const float _multiplier, const Time _time)			{ return Time(TimeValueType((float)_time.value * _multiplier)); }
		constexpr Time operator*(const int _multiplier, const Time _time)			{ return Time(_time.value * (TimeValueType)_multiplier); }
		constexpr Time operator*(const TimeValueType _multiplier, const Time _time)	{ return Time(_time.value * _multiplier); }

		template<typename T = float> constexpr T asSeconds(const Time _time)		{ return _time.asSeconds<T>(); }
		template<typename T = float> constexpr T asMilliseconds(const Time _time)	{ return _time.asMilliseconds<T>(); }
		template<typename T = float> constexpr T asMicroseconds(const Time _time)	{ return _time.asMicroseconds<T>(); }
		template<typename T = float> constexpr T asNanoseconds(const Time _time)	{ return _time.asNanoseconds<T>(); }

		constexpr Time fromSeconds(const double _seconds)							{ return TimeValueType(_seconds * static_cast<double>(conversionRate::second)); }
		constexpr Time fromMilliseconds(const double _milliseconds)					{ return TimeValueType(_milliseconds * static_cast<double>(conversionRate::millisecond)); }
		constexpr Time fromMicroseconds(const double _microseconds)					{ return TimeValueType(_microseconds * static_cast<double>(conversionRate::microsecond)); }
		constexpr Time fromNanoseconds(const double _nanoseconds)					{ return TimeValueType(_nanoseconds * static_cast<double>(conversionRate::nanosecond)); }

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