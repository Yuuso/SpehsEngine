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

			void operator*=(const float factor);
			void operator*=(const int factor);
			void operator/=(const float divisor);
			void operator/=(const int divisor);
			void operator+=(const Time& other);
			void operator-=(const Time& other);
			Time operator*(const float _asSeconds) const;
			Time operator*(const int _asMilliseconds) const;
			Time operator/(const float _asSeconds) const;
			Time operator/(const int _asMilliseconds) const;
			Time operator+(const Time& other) const;
			Time operator-(const Time& other) const;
			void operator=(const Time& other);
			bool operator==(const Time& other) const;
			bool operator>(const Time& other) const;
			bool operator<(const Time& other) const;
			bool operator>=(const Time& other) const;
			bool operator<=(const Time& other) const;

			inline float asSeconds() const
			{
				return (float)value / (float)conversionRate::second;
			}
			inline float asMilliseconds() const
			{
				return (float)value / conversionRate::millisecond;
			}
			inline float asMicroseconds() const
			{
				return (float)value / (float)conversionRate::microsecond;
			}
			inline float asNanoseconds() const
			{
				return (float)value / (float)conversionRate::nanosecond;
			}
			
			TimeValueType value;
		};
		
		inline Time seconds(const float seconds)
		{
			return seconds * (float)conversionRate::second;
		}
		inline Time milliseconds(const float milliseconds)
		{
			return milliseconds * (float)conversionRate::millisecond;
		}
		inline Time microseconds(const float microseconds)
		{
			return microseconds * (float)conversionRate::microsecond;
		}
		inline Time nanoseconds(const int64_t nanoseconds)
		{
			return nanoseconds * (float)conversionRate::nanosecond;
		}

		/* Initializes the time system. */
		void initialize();

		/* Waits in a while loop until specified time has passed. */
		void delay(const Time& time);

		/* Returns the current time stamp, relative to 'some' context. See getRunTime() for an alternative. */
		Time now();

		/* Returns current time since time was initialized. Less efficient than using now(). */
		Time getRunTime();

		//Get time and date of when the engine was built / TODO: this actually only ever updates when the time source files change/on complete rebuild?
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