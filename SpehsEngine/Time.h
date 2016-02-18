#pragma once
#include <stdint.h>


namespace SpehsEngine
{
	//Variables that should be visible outside
	extern float fps;
	extern int maxFPS;
	extern uint32_t maxDeltaTime;
	extern uint32_t deltaTime;
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