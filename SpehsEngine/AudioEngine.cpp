
#include "AudioEngine.h"
#include "Exceptions.h"
#include "OpenALError.h"
#include "SoundSource.h"
#include "Console.h"

#include <AL\al.h>
#include <AL\alc.h>

#include <algorithm>


#define DEFAULT_MAX_SOURCES 256


namespace spehs
{
	namespace audio
	{
		AudioEngine* AudioEngine::instance = nullptr;
		void AudioEngine::init()
		{
			if (!AudioEngine::instance)
			{
				AudioEngine::instance = new AudioEngine;
			}
			else
			{
				exceptions::fatalError("Audio engine already initialized!");
			}
		}
		void AudioEngine::uninit()
		{
			if (AudioEngine::instance)
			{
				delete AudioEngine::instance;
			}
			else
			{
				exceptions::warning("Cannot uninit audio engine, instance not initialized!");
			}
		}

		void AudioEngine::setMaxSources(const unsigned int _maxSources)
		{
			AudioEngine::instance->maxSources = _maxSources;
		}

		void AudioEngine::setListenerPosition(const glm::vec2& _pos)
		{
			AudioEngine::instance->listenerPosition = _pos;
			alListener3f(AL_POSITION, _pos.x, _pos.y, 0.0f);
		}
		void AudioEngine::setListenerVelocity(const glm::vec2& _vel)
		{
			AudioEngine::instance->listenerVelocity = _vel;
			alListener3f(AL_VELOCITY, _vel.x, _vel.y, 0.0f);
		}
		void AudioEngine::setListenerGain(const float _gain)
		{
			AudioEngine::instance->listenerGain = _gain;
			alListenerf(AL_GAIN, _gain);
		}
		glm::vec2 AudioEngine::getListenerPosition()
		{
			return AudioEngine::instance->listenerPosition;
		}
		glm::vec2 AudioEngine::getListenerVelocity()
		{
			return AudioEngine::instance->listenerVelocity;
		}
		float AudioEngine::getListenerGain()
		{
			return AudioEngine::instance->listenerGain;
		}


		AudioEngine::AudioEngine() : maxSources(DEFAULT_MAX_SOURCES), listenerGain(1.0f), listenerPosition(0.0f), listenerVelocity(0.0f)
		{
			device = alcOpenDevice(NULL);
			if (!device)
			{
				exceptions::fatalError("Opening OpenAL device failed!");
			}

			context = alcCreateContext(device, NULL);
			if (!alcMakeContextCurrent(context))
			{
				exceptions::fatalError("Failed to make OpenAL context current!");
			}

			checkOpenALCError(device, __FILE__, __LINE__);

			for (unsigned i = 0; i < 16; i++) //Bit of a messy solution but this isn't called very often so what ever.
				sourcePool.push_back(new SourceObject());
		}
		AudioEngine::~AudioEngine()
		{
			device = alcGetContextsDevice(context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(context);
			alcCloseDevice(device);
		}

		bool AudioEngine::getFreeSource(SoundSource* _soundSource)
		{
			for (unsigned i = 0; i < sourcePool.size(); i++)
			{
				if (!sourcePool[i]->soundPtr)
				{
					if (sourcePool[i]->sourceID != 0)
					{
						//Found free source!
						sourcePool[i]->soundPtr = _soundSource;
						_soundSource->source = sourcePool[i];
						return true;
					}
					else
					{
						//Need to generate source
						alGenSources(1, &sourcePool[i]->sourceID);
						checkOpenALErrors(__FILE__, __LINE__);
						if (sourcePool[i]->sourceID != 0)
						{
							sourcePool[i]->soundPtr = _soundSource;
							_soundSource->source = sourcePool[i];
							return true;
						}
						assert(sourcePool[i]->sourceID);//FOR DEBUG TESTING PURPOSES ONLY REMOVE LATER!!!!!
					}
				}
			}

			if (sourcePool.size() < maxSources)
			{
				//Try making more sources
				int amount = std::min(maxSources, sourcePool.size() * 2);
				for (unsigned i = sourcePool.size(); i < amount; i++) //Bit of a messy solution but this isn't called very often so what ever.
					sourcePool.push_back(new SourceObject());
				return getFreeSource(_soundSource);
			}

			//If nothing else > steal it from someone with 'lower' priority
			std::sort(sourcePool.begin(), sourcePool.end(), [](SourceObject* _a, SourceObject* _b)
			{
				return _a->soundPtr->getPriority() > _b->soundPtr->getPriority();
			});
			for (unsigned i = 0; i < sourcePool.size(); i++)
			{
				if (sourcePool[i]->soundPtr->getPriority() > _soundSource->getPriority())
				{
					if (sourcePool[i]->soundPtr->getPriority() != 0)
					{
						sourcePool[i]->soundPtr->removeSource();
						sourcePool[i]->soundPtr = _soundSource;
						_soundSource->source = sourcePool[i];
						return true;
					}
				}
			}

			//This should be quite rare
			spehs::console::warning("Could not find a free sound source object!");
			return false;
		}
	}
}