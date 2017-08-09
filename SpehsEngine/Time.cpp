
#include <SDL\SDL_timer.h>
#include <iostream>
#include <mutex>
#include <algorithm>

#include "ApplicationData.h"
#include "BatchManager.h"
#include "Camera2D.h"
#include "Time.h"
#include "Text.h"


namespace spehs
{
	namespace time
	{
		//Global variables
		Time maxDeltaTime(0);

		//Mutex
		std::mutex deltaTimeMutex;
		std::mutex runTimeMutex;
		std::mutex FPSMutex;

		//Local variables
		Time deltaTime;
		Time runTime;
		uint32_t startTicks = 0;
		float fps = 0;

		void update()
		{
			static const int NUM_SAMPLES = 20;
			static uint32_t deltaTimes[NUM_SAMPLES];
			static int currentFrame = 0;
			static uint32_t previousTicks = SDL_GetTicks();
			static uint32_t currentTicks;

			runTimeMutex.lock();
			runTime = (int) SDL_GetTicks();
			runTimeMutex.unlock();
			currentTicks = SDL_GetTicks();

			deltaTimeMutex.lock();
			deltaTime = int(currentTicks - previousTicks);
			deltaTime = std::max(1, deltaTime.asMilliseconds);
			deltaTimes[currentFrame % NUM_SAMPLES] = deltaTime.asMilliseconds;
			deltaTimeMutex.unlock();

			previousTicks = currentTicks;

			//Limit delta time
			if (maxDeltaTime.asMilliseconds > 0 && deltaTime > maxDeltaTime)
			{
				deltaTimeMutex.lock();
				deltaTime = maxDeltaTime;
				deltaTimeMutex.unlock();
			}

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

			static float deltaTimeAverage;
			deltaTimeAverage = 0;
			for (int i = 0; i < count; i++)
			{
				deltaTimeAverage += deltaTimes[i];
			}
			deltaTimeAverage /= count;

			FPSMutex.lock();
			if (deltaTimeAverage > 0)
			{
				fps = 1000.0f / deltaTimeAverage;
			}
			else
			{
				fps = 0;
			}
			FPSMutex.unlock();

			//Limit FPS = delay return
			if (spehs::ApplicationData::maxFps > 0)
			{
				if ((1000.0f / spehs::ApplicationData::maxFps) > deltaTime.asMilliseconds)
					SDL_Delay(uint32_t(1000.0f / spehs::ApplicationData::maxFps) - deltaTime.asMilliseconds);
			}
		}

		Time getDeltaTime()
		{
			std::lock_guard<std::mutex> lock(deltaTimeMutex);
			return deltaTime;
		}
		float getDeltaTimeAsSeconds()
		{
			std::lock_guard<std::mutex> lock(deltaTimeMutex);
			return deltaTime.asSeconds;
		}
		int getDeltaTimeAsMilliseconds()
		{
			std::lock_guard<std::mutex> lock(deltaTimeMutex);
			return deltaTime.asMilliseconds;
		}
		Time getRunTime()
		{
			std::lock_guard<std::mutex> lock(runTimeMutex);
			return runTime;
		}
		float getFPS()
		{
			std::lock_guard<std::mutex> lock(FPSMutex);
			return fps;
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
#pragma region TIME STRUCT
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
#pragma endregion
	}
}