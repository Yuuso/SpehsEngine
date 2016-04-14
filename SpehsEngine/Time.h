#pragma once
#include <stdint.h>


namespace spehs
{
	struct Time
	{
		Time();
		Time(const Time& other);
		Time(const float _asSeconds);
		Time(const uint32_t _asMilliSeconds);
		void operator*=(const Time& other);
		void operator*=(const float& _asSeconds);
		void operator*=(const uint32_t& _asMilliseconds);
		void operator/=(const Time& other);
		void operator/=(const float& _asSeconds);
		void operator/=(const uint32_t& _asMilliseconds);
		void operator+=(const Time& other);
		void operator+=(const float& _asSeconds);
		void operator+=(const uint32_t& _asMilliseconds);
		void operator-=(const Time& other);
		void operator-=(const float& _asSeconds);
		void operator-=(const uint32_t& _asMilliseconds);
		Time operator*(const Time& other);
		Time operator*(const float& _asSeconds);
		Time operator*(const uint32_t& _asMilliseconds);
		Time operator/(const Time& other);
		Time operator/(const float& _asSeconds);
		Time operator/(const uint32_t& _asMilliseconds);
		Time operator+(const Time& other);
		Time operator+(const float& _asSeconds);
		Time operator+(const uint32_t& _asMilliseconds);
		Time operator-(const Time& other);
		Time operator-(const float& _asSeconds);
		Time operator-(const uint32_t& _asMilliseconds);
		void operator=(const Time& other);
		void operator=(const float _asSeconds);
		void operator=(const uint32_t _asMilliseconds);
		bool operator==(const Time& other);
		bool operator==(const float _asSeconds);
		bool operator==(const uint32_t _asMilliseconds);
		bool operator>(const Time& other);
		bool operator>(const float _asSeconds);
		bool operator>(const uint32_t _asMilliseconds);
		bool operator<(const Time& other);
		bool operator<(const float _asSeconds);
		bool operator<(const uint32_t _asMilliseconds);
		bool operator>=(const Time& other);
		bool operator>=(const float _asSeconds);
		bool operator>=(const uint32_t _asMilliseconds);
		bool operator<=(const Time& other);
		bool operator<=(const float _asSeconds);
		bool operator<=(const uint32_t _asMilliseconds);
		uint32_t asMilliseconds;
		float asSeconds;
	};
	//Variables that should be visible outside
	extern float fps;
	extern Time maxDeltaTime;
	extern Time deltaTime;
	extern unsigned long drawCalls;
	extern unsigned long vertexDrawCount;

	bool initializeTime();
	void uninitializeTime();
	void beginFPS();
	void endFPS();
	void drawFPS();

	//Analyzation
	/*30 timers (0-29)*/
	void beginTimer(int timerIndex);
	uint32_t endTimer(int timerIndex);
}