#pragma once

#include <stdint.h>
#include <string>

#ifdef _DEBUG
#define SPEHS_DEBUG_DURATION_BEGIN(timeDurationVariable) timeDurationVariable = spehs::time::now();
#define SPEHS_DEBUG_DURATION_END(timeDurationVariable) timeDurationVariable = spehs::time::now() - timeDurationVariable;
#else
#define SPEHS_DEBUG_DURATION_BEGIN(timeDurationVariable) (void)0
#define SPEHS_DEBUG_DURATION_END(timeDurationVariable) (void)0
#endif

#ifdef delay
#undef delay
#endif

namespace spehs
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

		struct Time
		{
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
			bool operator>(const Time& other) const;
			bool operator<(const Time& other) const;
			bool operator>=(const Time& other) const;
			bool operator<=(const Time& other) const;

			operator bool() const { return value != 0; }
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
		
		inline Time fromSeconds(const float seconds) { return seconds * (float)conversionRate::second; }
		inline Time fromMilliseconds(const float milliseconds) { return milliseconds * (float)conversionRate::millisecond; }
		inline Time fromMicroseconds(const float microseconds) { return microseconds * (float)conversionRate::microsecond; }
		inline Time fromNanoseconds(const int64_t nanoseconds) { return nanoseconds * (float)conversionRate::nanosecond; }

		static const Time zero(0);
		static const Time second(conversionRate::second);
		static const Time millisecond(conversionRate::millisecond);
		static const Time microsecond(conversionRate::microsecond);
		static const Time nanosecond(conversionRate::nanosecond);

		/* Initializes the time system. */
		void initialize();

		/* Waits in a while loop until specified time has passed. */
		void delay(const Time time);


		/* Returns the current time stamp, relative to 'some' context. See getRunTime() for an alternative. */
		Time now();

		/* Returns current time since time was initialized. Less efficient than using now(). */
		Time getRunTime();

		/* Returns time when spehs::time was initialized. */
		Time getInitializationTime();

		//Get time and date of when the engine was built
		//TODO: this actually only ever updates when the time source files are being rebuilt. And with the addition of split projects, this is ever less usefull.
		std::string engineBuildYear();
		std::string engineBuildMonth();
		std::string engineBuildDay();
		std::string engineBuildHour();
		std::string engineBuildMinute();
		std::string engineBuildSecond();

		class DeltaTimeSystem
		{
		public:
			DeltaTimeSystem(const std::string& debugName = "unnamed")
				: deltaSeconds(0.0f)
				, deltaTime(0)
				, name(debugName)
				, deltaTimestamp(now())
			{

			}
			virtual ~DeltaTimeSystem()
			{

			}
			void deltaTimeSystemInitialize()
			{
				deltaSeconds = 0.0f;
				deltaTime.value = 0;
				deltaTimestamp = now();
			}
			void deltaTimeSystemUpdate()
			{
				const spehs::time::Time now = spehs::time::now();
				deltaTime = now - deltaTimestamp;
				deltaTimestamp = now;
				deltaSeconds = deltaTime.asSeconds();
			}

			//Public attributes for maximum speed
			float deltaSeconds;
			spehs::time::Time deltaTime;

		private:
			std::string name;
			spehs::time::Time deltaTimestamp;
		};

		/* Records time at the time of allocation. Elapsed time can be retrieved using get() at any time. */
		class ScopeTimer
		{
		public:
			ScopeTimer() : time(now()) {}
			inline Time get() const { return now() - time; }
		private:
			Time time;
		};

		/* get() returns time since last get(), or time since allocation if get() hasn't been used. */
		class LapTimer
		{
		public:
			LapTimer() : time(now()) {}
			Time get()
			{
				const Time n = now();
				const Time lapTime = now() - time;
				time = n;
				return lapTime;
			}
		private:
			Time time;
		};
	}
}