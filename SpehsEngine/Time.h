
#pragma once

#include <stdint.h>
#include <string>


namespace spehs
{
	namespace time
	{
		struct Time
		{
			Time();
			Time(const Time& other);
			Time(const float _asSeconds);
			Time(const int _asMilliSeconds);
			void operator*=(const Time& other);
			void operator*=(const float _asSeconds);
			void operator*=(const int _asMilliseconds);
			void operator/=(const Time& other);
			void operator/=(const float _asSeconds);
			void operator/=(const int _asMilliseconds);
			void operator+=(const Time& other);
			void operator+=(const float _asSeconds);
			void operator+=(const int _asMilliseconds);
			void operator-=(const Time& other);
			void operator-=(const float _asSeconds);
			void operator-=(const int _asMilliseconds);
			Time operator*(const Time& other);
			Time operator*(const float _asSeconds);
			Time operator*(const int _asMilliseconds);
			Time operator/(const Time& other);
			Time operator/(const float _asSeconds);
			Time operator/(const int _asMilliseconds);
			Time operator+(const Time& other);
			Time operator+(const float _asSeconds);
			Time operator+(const int _asMilliseconds);
			Time operator-(const Time& other);
			Time operator-(const float _asSeconds);
			Time operator-(const int _asMilliseconds);
			void operator=(const Time& other);
			void operator=(const float _asSeconds);
			void operator=(const int _asMilliseconds);
			bool operator==(const Time& other);
			bool operator==(const float _asSeconds);
			bool operator==(const int _asMilliseconds);
			bool operator>(const Time& other);
			bool operator>(const float _asSeconds);
			bool operator>(const int _asMilliseconds);
			bool operator<(const Time& other);
			bool operator<(const float _asSeconds);
			bool operator<(const int _asMilliseconds);
			bool operator>=(const Time& other);
			bool operator>=(const float _asSeconds);
			bool operator>=(const int _asMilliseconds);
			bool operator<=(const Time& other);
			bool operator<=(const float _asSeconds);
			bool operator<=(const int _asMilliseconds);
			int asMilliseconds;
			float asSeconds;
		};

		void update();

		Time getDeltaTime();
		float getDeltaTimeAsSeconds();
		int getDeltaTimeAsMilliseconds();
		Time getRunTime();///< For how long the program has run.
		float getFPS();

		//Get time and date of when the engine was built
		std::string engineBuildYear();
		std::string engineBuildMonth();
		std::string engineBuildDay();
		std::string engineBuildHour();
		std::string engineBuildMinute();
		std::string engineBuildSecond();
	}
}