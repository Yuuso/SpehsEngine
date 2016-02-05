#pragma once
#include <SDL\SDL_stdinc.h>//Uint32



namespace SpehsEngine
{
	//Variables that should be visible outside
	extern float fps;
	extern int maxFPS;
	extern Uint32 deltaTime;
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
	Uint32 endTimer(int timerIndex);

}