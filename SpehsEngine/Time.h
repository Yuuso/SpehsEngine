
#pragma once

#include <stdint.h>
#include <string>


namespace spehs
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
	//Variables that should be visible outside
	extern float fps;
	extern Time maxDeltaTime;
	Time getDeltaTime();
	Time getRunTime();///< For how long the program has run. Updates from endFPS()
	extern unsigned long drawCalls;
	extern unsigned long vertexDrawCount;

	//Get time and date of when the engine was built
	std::string engineBuildYear();
	std::string engineBuildMonth();
	std::string engineBuildDay();
	std::string engineBuildHour();
	std::string engineBuildMinute();
	std::string engineBuildSecond();

	bool initializeTime();
	void uninitializeTime();
	void beginFPS();
	void endFPS();
	void drawFPS();
}