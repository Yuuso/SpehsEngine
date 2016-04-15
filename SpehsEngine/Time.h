#pragma once
#include <stdint.h>


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

	bool initializeTime();
	void uninitializeTime();
	void beginFPS();
	void endFPS();
	void drawFPS();
}