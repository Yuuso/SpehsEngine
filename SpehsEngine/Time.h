#pragma once
#include <stdint.h>


namespace spehs
{
	struct Time
	{
		Time() : asMilliseconds(0), asSeconds(0) {}
		Time(const float _asSeconds) : asMilliseconds(_asSeconds * 1000), asSeconds(_asSeconds) {}
		Time(const uint32_t _asMilliSeconds) : asMilliseconds(_asMilliSeconds), asSeconds(float(_asMilliSeconds) / 1000.0f) {}
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