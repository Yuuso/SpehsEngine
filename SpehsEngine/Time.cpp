#include <SDL\SDL_timer.h>
#include <iostream>

#include "ApplicationData.h"
#include "Time.h"
#include "Text.h"

#define FPS_REFRESH_RATE 5
#define FPS_FONT_PATH "Fonts/Anonymous.ttf"


namespace SpehsEngine
{
	//Global variables
	int fpsCounterFontSize = 24;
	float fps = 0;
	int maxFPS = 0;
	Uint32 deltaTime = 0;
	unsigned long drawCalls;
	unsigned long vertexDrawCount;

	//Local variables
	float runTime = 0;
	Uint32 startTicks = 0;
	SpehsEngine::Text* fpsCounter;
	static bool initialized = false;
	static int previousFontSize = fpsCounterFontSize;



	bool initializeTime()
	{
		if (initialized)
		{
			std::cout << "\nTime already initialized!";
			return true;
		}

		fpsCounter = new SpehsEngine::Text();
		if (!fpsCounter)
		{
			std::cout << "\nInitialization failed! Failed to create fpsCounter!";
			return false;
		}

		if (!fpsCounter->setFont(FPS_FONT_PATH, fpsCounterFontSize))
		{
			std::cout << "\nInitialization failed! Failed to set up fps counter!";
			return false;
		}

		fpsCounter->setColor(glm::vec4(1.0f, 0.3f, 0.0f, 1.0f));
		fpsCounter->setPosition(glm::vec2(5, applicationData->getWindowHeight() - fpsCounter->getFontHeight()));

		initialized = true;
		return true;
	}
	void uninitializeTime()
	{
		if (initialized == false)
			return;

		if (fpsCounter != nullptr)
		{
			delete fpsCounter;
			fpsCounter = nullptr;
		}

		initialized = false;
	}

	void beginFPS()
	{
		startTicks = SDL_GetTicks();
		runTime += deltaTime / 1000.0f;

		//Update fps counter font size if needed
		if (previousFontSize != fpsCounterFontSize)
		{
			fpsCounter->setFont(FPS_FONT_PATH, fpsCounterFontSize);
			previousFontSize = fpsCounterFontSize;
		}
	}
	void endFPS()
	{
		static const int NUM_SAMPLES = 20;
		static Uint32 deltaTimes[NUM_SAMPLES];
		static int currentFrame = 0;

		static Uint32 previousTicks = SDL_GetTicks();

		Uint32 currentTicks;
		currentTicks = SDL_GetTicks();

		deltaTime = currentTicks - previousTicks;
		deltaTimes[currentFrame % NUM_SAMPLES] = deltaTime;

		previousTicks = currentTicks;

		int count;

		currentFrame++;
		if (currentFrame < NUM_SAMPLES)
		{
			count = currentFrame;
		}
		else
		{
			count = NUM_SAMPLES;
		}

		float deltaTimeAverage = 0;
		for (int i = 0; i < count; i++)
		{
			deltaTimeAverage += deltaTimes[i];
		}
		deltaTimeAverage /= count;

		if (deltaTimeAverage > 0)
		{
			fps = 1000.0f / deltaTimeAverage;
		}
		else
		{
			fps = 0;
		}

		static int frameCounter = 0;
		if (++frameCounter >= FPS_REFRESH_RATE)
		{
			fpsCounter->setString("FPS: " + std::to_string(int(fps)) + "\nDraw calls: " + std::to_string(drawCalls) + "\nVertices: " + std::to_string(vertexDrawCount));
			frameCounter = 0;
		}

		drawCalls = 0;
		vertexDrawCount = 0;

		//Limit FPS = delay return
		Uint32 frameTicks = SDL_GetTicks() - startTicks;
		if (maxFPS > 0)
			if (1000.0f / maxFPS > frameTicks)
				SDL_Delay(Uint32(1000.0f / maxFPS) - frameTicks);

	}
	void drawFPS()
	{
		if (!applicationData->showFps)
			return;
		fpsCounter->render();
	}

	//Analyzation
	static Uint32 beginTime[30];
	static Uint32 endTime[30];
	static bool timerOn = false;
	void beginTimer(int timerIndex)
	{
		if (timerOn == true)
			return;
		timerOn = true;
		beginTime[timerIndex] = SDL_GetTicks();
	}
	Uint32 endTimer(int timerIndex)
	{
		if (timerOn == false)
			return -1;
		timerOn = false;
		endTime[timerIndex] = SDL_GetTicks();

		return endTime[timerIndex] - beginTime[timerIndex];
	}
}