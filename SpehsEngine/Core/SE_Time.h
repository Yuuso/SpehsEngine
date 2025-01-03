#pragma once

#ifdef delay
#undef delay
#endif


namespace se
{
	typedef int64_t TimeValueType;
	namespace detail
	{
		namespace time_conversion_rate
		{
			constexpr TimeValueType second = 1000000000;
			constexpr TimeValueType millisecond = second / 1000;
			constexpr TimeValueType microsecond = millisecond / 1000;
			constexpr TimeValueType nanosecond = microsecond / 1000;
		}
	}

	class Time
	{
	public:

		static const Time zero;

		// Initializes the time system.
		static void initialize();

		static constexpr Time fromSeconds(const double _s)				{ return TimeValueType(_s * static_cast<double>(detail::time_conversion_rate::second)); }
		static constexpr Time fromMilliseconds(const double _ms)		{ return TimeValueType(_ms * static_cast<double>(detail::time_conversion_rate::millisecond)); }
		static constexpr Time fromMicroseconds(const double _us)		{ return TimeValueType(_us * static_cast<double>(detail::time_conversion_rate::microsecond)); }
		static constexpr Time fromNanoseconds(const double _ns)			{ return TimeValueType(_ns * static_cast<double>(detail::time_conversion_rate::nanosecond)); }

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

		template<typename T = float> constexpr T asSeconds() const		{ return (T)value / (T)detail::time_conversion_rate::second; }
		template<typename T = float> constexpr T asMilliseconds() const	{ return (T)value / (T)detail::time_conversion_rate::millisecond; }
		template<typename T = float> constexpr T asMicroseconds() const	{ return (T)value / (T)detail::time_conversion_rate::microsecond; }
		template<typename T = float> constexpr T asNanoseconds() const	{ return (T)value / (T)detail::time_conversion_rate::nanosecond; }

		TimeValueType value;
	};

	inline constexpr const Time Time::zero = Time(0);

	constexpr Time operator*(const double _multiplier, const Time _time)		{ return Time(TimeValueType((double)_time.value * _multiplier)); }
	constexpr Time operator*(const float _multiplier, const Time _time)			{ return Time(TimeValueType((float)_time.value * _multiplier)); }
	constexpr Time operator*(const int _multiplier, const Time _time)			{ return Time(_time.value * (TimeValueType)_multiplier); }
	constexpr Time operator*(const TimeValueType _multiplier, const Time _time)	{ return Time(_time.value * _multiplier); }

	// Puts thread to sleep for (at least) this much time.
	void sleep(const Time time);

	// Waits in a while loop until specified time has passed. More precise than sleep, but hogs processor for the duration of delay.
	void delay(const Time time);

	// Get date and time string in strftime format.
	std::string getDateString(const std::string& format);

	// Returns current time since epoch.
	Time getEpochTime();
	inline Time getEpochTimeSince(const Time _epochTime) { return getEpochTime() - _epochTime; }

	// Returns system time (since epoch). System time is not as precise as getEpochTime(). Note that the user is able to change system time.
	Time getSystemTime();
	inline Time getSystemTimeSince(const Time _systemTime) { return getEpochTime() - _systemTime; }

	// Potentially more efficient than getEpochTime(). Prefer getEpochTime() for non-profiling purposes.
	Time getProfilerTime();
	inline Time getProfilerTimeSince(const Time _profilerTime) { return getProfilerTime() - _profilerTime; }

	// Returns time since calling Time::initialize(). Less efficient than using getEpochTime().
	Time getRunTime();
	inline Time getRunTimeSince(const Time _runTime) { return getRunTime() - _runTime; }
}