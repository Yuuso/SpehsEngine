#include <SDL\SDL_timer.h>
#include <iostream>
#include <mutex>

#include "ApplicationData.h"
#include "Time.h"
#include "Text.h"

#define FPS_REFRESH_RATE 5
#define FPS_FONT_PATH "Fonts/Anonymous.ttf"


namespace spehs
{
	//Global variables
	int fpsCounterFontSize = 24;
	float fps = 0;
	Time maxDeltaTime(1000);
	unsigned long drawCalls;
	unsigned long vertexDrawCount;

	//Mutex
	std::mutex deltaTimeMutex;
	std::mutex runTimeMutex;

	//Local variables
	Time deltaTime;
	Time runTime; 
	uint32_t startTicks = 0;
	spehs::Text* fpsCounter;
	static bool initialized = false;
	static int previousFontSize = fpsCounterFontSize;



	bool initializeTime()
	{
		if (initialized)
		{
			std::cout << "\nTime already initialized!";
			return true;
		}

		fpsCounter = new spehs::Text(10000);
		if (!fpsCounter)
		{
			std::cout << "\nInitialization failed! Failed to create fpsCounter!";
			return false;
		}

		fpsCounterFontSize = applicationData->consoleTextSize;
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
		static uint32_t deltaTimes[NUM_SAMPLES];
		static int currentFrame = 0;
		static uint32_t previousTicks = SDL_GetTicks();

		runTime = (int)SDL_GetTicks();
		uint32_t currentTicks;
		currentTicks = SDL_GetTicks();

		deltaTime.asMilliseconds = currentTicks - previousTicks;
		deltaTime.asSeconds = float(deltaTime.asMilliseconds) / 1000.0f;
		deltaTimes[currentFrame % NUM_SAMPLES] = deltaTime.asMilliseconds;

		//Limit delta time
		if (maxDeltaTime.asMilliseconds > 0 && deltaTime.asMilliseconds > maxDeltaTime.asMilliseconds)
		{
			deltaTime.asMilliseconds = maxDeltaTime.asMilliseconds;
			deltaTime.asSeconds = maxDeltaTime.asSeconds;
		}

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
		uint32_t frameTicks = SDL_GetTicks() - startTicks;
		if (applicationData->maxFps > 0)
			if (1000.0f / applicationData->maxFps > frameTicks)
				SDL_Delay(uint32_t(1000.0f / applicationData->maxFps) - frameTicks);

	}
	void drawFPS()
	{
		if (!applicationData->showFps)
			return;
		fpsCounter->setRenderState(true);
	}
	Time getDeltaTime()
	{
		std::lock_guard<std::mutex> lock(deltaTimeMutex);
		return deltaTime;
	}
	Time getRunTime()
	{
		std::lock_guard<std::mutex> lock(runTimeMutex);
		return runTime;
	}


	std::string engineBuildYear()
	{
		std::string year;
		year.push_back(__DATE__[7]);
		year.push_back(__DATE__[8]);
		year.push_back(__DATE__[9]);
		year.push_back(__DATE__[10]);
		return year;
	}
	std::string engineBuildMonth()
	{
		return __DATE__[2] == 'n' ? (__DATE__[1] == 'a' ? "01" : "06") 
			: __DATE__[2] == 'b' ? "02" 
			: __DATE__[2] == 'r' ? (__DATE__[0] == 'M' ? "03" : "04") 
			: __DATE__[2] == 'y' ? "05" 
			: __DATE__[2] == 'l' ? "07" 
			: __DATE__[2] == 'g' ? "08" 
			: __DATE__[2] == 'p' ? "09" 
			: __DATE__[2] == 't' ? "10" 
			: __DATE__[2] == 'v' ? "11" 
			: "12";
	}
	std::string engineBuildDay()
	{
		std::string day;
		day.push_back((__DATE__[4] == ' ' ? '0' : __DATE__[4]));
		day.push_back(__DATE__[5]);
		return day;
	}
	std::string engineBuildHour()
	{
		std::string hour;
		hour.push_back(__TIME__[0]);
		hour.push_back(__TIME__[1]);
		return hour;
	}
	std::string engineBuildMinute()
	{
		std::string minute;
		minute.push_back(__TIME__[3]);
		minute.push_back(__TIME__[4]);
		return minute;
	}
	std::string engineBuildSecond()
	{
		std::string second;
		second.push_back(__TIME__[6]);
		second.push_back(__TIME__[7]);
		return second;
	}





	//TIME STRUCT
	Time::Time() : asMilliseconds(0), asSeconds(0) {}
	Time::Time(const Time& other) : asMilliseconds(other.asMilliseconds), asSeconds(other.asSeconds) {}
	Time::Time(const float _asSeconds) : asMilliseconds(_asSeconds * 1000), asSeconds(_asSeconds) {}
	Time::Time(const int _asMilliSeconds) : asMilliseconds(_asMilliSeconds), asSeconds(float(_asMilliSeconds) / 1000.0f) {}
	Time Time::operator*(const Time& other)
	{
		return Time(asSeconds * other.asSeconds);
	}
	Time Time::operator*(const float _asSeconds)
	{
		return Time(asSeconds * _asSeconds);
	}
	Time Time::operator*(const int _asMilliseconds)
	{
		return Time(asSeconds * float(_asMilliseconds * 1000.0f));
	}
	Time Time::operator/(const Time& other)
	{
		return Time(asSeconds / other.asSeconds);
	}
	Time Time::operator/(const float _asSeconds)
	{
		return Time(asSeconds / _asSeconds);
	}
	Time Time::operator/(const int _asMilliseconds)
	{
		return Time(float(asMilliseconds * 1000) / float(_asMilliseconds * 1000));
	}
	Time Time::operator+(const Time& other)
	{
		return Time(asMilliseconds + other.asMilliseconds);
	}
	Time Time::operator+(const float _asSeconds)
	{
		return Time(asSeconds + _asSeconds);
	}
	Time Time::operator+(const int _asMilliseconds)
	{
		return Time(asMilliseconds + _asMilliseconds);
	}
	Time Time::operator-(const Time& other)
	{
		return Time(asMilliseconds - other.asMilliseconds);
	}
	Time Time::operator-(const float _asSeconds)
	{
		return Time(asSeconds - _asSeconds);
	}
	Time Time::operator-(const int _asMilliseconds)
	{
		return Time(asMilliseconds - _asMilliseconds);
	}
	void Time::operator=(const Time& other)
	{
		asMilliseconds = other.asMilliseconds;
		asSeconds = other.asSeconds;
	}
	void Time::operator=(const float _asSeconds)
	{
		asMilliseconds = _asSeconds * 1000.0f;
		asSeconds = _asSeconds;
	}
	void Time::operator=(const int _asMilliseconds)
	{
		asMilliseconds = _asMilliseconds;
		asSeconds = _asMilliseconds / 1000.0f;
	}
	void Time::operator*=(const Time& other)
	{
		asMilliseconds = float(asMilliseconds * 1000.0f) * float(other.asMilliseconds * 1000.0f);
		asSeconds *= other.asSeconds;
	}
	void Time::operator*=(const float _asSeconds)
	{
		asMilliseconds = float(asMilliseconds * 1000.0f) * _asSeconds;
		asSeconds *= _asSeconds;
	}
	void Time::operator*=(const int _asMilliseconds)
	{
		asMilliseconds = float(asMilliseconds * 1000.0f) * float(_asMilliseconds * 1000.0f);
		asSeconds *= float(_asMilliseconds * 1000.0f);
	}
	void Time::operator/=(const Time& other)
	{
		asMilliseconds = float(asMilliseconds * 1000.0f) / float(other.asMilliseconds * 1000.0f);
		asSeconds /= other.asSeconds;
	}
	void Time::operator/=(const float _asSeconds)
	{
		asMilliseconds = float(asMilliseconds * 1000.0f) / _asSeconds;
		asSeconds /= _asSeconds;
	}
	void Time::operator/=(const int _asMilliseconds)
	{
		asMilliseconds = float(asMilliseconds * 1000.0f) / float(_asMilliseconds * 1000.0f);
		asSeconds /= float(_asMilliseconds * 1000.0f);
	}
	void Time::operator+=(const Time& other)
	{
		asSeconds += other.asSeconds;
		asMilliseconds += other.asMilliseconds;
	}
	void Time::operator+=(const float _asSeconds)
	{
		asSeconds += _asSeconds;
		asMilliseconds += int(_asSeconds * 1000.0f);
	}
	void Time::operator+=(const int _asMilliseconds)
	{
		asSeconds += float(_asMilliseconds) / 1000.0f;
		asMilliseconds += _asMilliseconds;
	}
	void Time::operator-=(const Time& other)
	{
		asSeconds -= other.asSeconds;
		asMilliseconds -= other.asMilliseconds;
	}
	void Time::operator-=(const float _asSeconds)
	{
		asSeconds -= _asSeconds;
		asMilliseconds -= int(_asSeconds * 1000.0f);
	}
	void Time::operator-=(const int _asMilliseconds)
	{
		asSeconds -= float(_asMilliseconds) / 1000.0f;
		asMilliseconds -= _asMilliseconds;
	}
	bool Time::operator==(const Time& other)
	{
		return asMilliseconds == other.asMilliseconds;
	}
	bool Time::operator==(const float _asSeconds)
	{
		return abs(asSeconds - _asSeconds) < 0.0005;
	}
	bool Time::operator==(const int _asMilliseconds)
	{
		return asMilliseconds == _asMilliseconds;
	}
	bool Time::operator>(const Time& other)
	{
		return asMilliseconds > other.asMilliseconds;
	}
	bool Time::operator>(const float _asSeconds)
	{
		return _asSeconds > _asSeconds;
	}
	bool Time::operator>(const int _asMilliseconds)
	{
		return asMilliseconds > _asMilliseconds;
	}
	bool Time::operator<(const Time& other)
	{
		return asMilliseconds < other.asMilliseconds;
	}
	bool Time::operator<(const float _asSeconds)
	{
		return _asSeconds < _asSeconds;
	}
	bool Time::operator<(const int _asMilliseconds)
	{
		return asMilliseconds < _asMilliseconds;
	}
	bool Time::operator>=(const Time& other)
	{
		return asMilliseconds >= other.asMilliseconds;
	}
	bool Time::operator>=(const float _asSeconds)
	{
		return asSeconds >= _asSeconds;
	}
	bool Time::operator>=(const int _asMilliseconds)
	{
		return asMilliseconds >= _asMilliseconds;
	}
	bool Time::operator<=(const Time& other)
	{
		return asMilliseconds <= other.asMilliseconds;
	}
	bool Time::operator<=(const float _asSeconds)
	{
		return asSeconds <= _asSeconds;
	}
	bool Time::operator<=(const int _asMilliseconds)
	{
		return asMilliseconds <= _asMilliseconds;
	}
}