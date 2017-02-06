
#include "AudioEngine.h"
#include "Exceptions.h"
#include "OpenALError.h"
#include "SoundSource.h"
#include "Console.h"
#include "ApplicationData.h"

#include <AL\al.h>
#include <AL\alc.h>

#include <algorithm>


#define DEFAULT_MAX_SOURCES 256


glm::vec2 positionCorrectionFactor = glm::vec2(1.0f);
float scaleCorrectionFactor = 1.0f;

extern const float defaultRollOffFactor = 1.75f;


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

			//alListenerf(AL_ROLLOFF_FACTOR, 500.0f);
			alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
		}
		void AudioEngine::uninit()
		{
			if (AudioEngine::instance)
			{
				delete AudioEngine::instance;
				AudioEngine::instance = nullptr;
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
			AudioEngine::instance->listenerPosition.x = _pos.x;
			AudioEngine::instance->listenerPosition.y = _pos.y;
			alListener3f(AL_POSITION, _pos.x * positionCorrectionFactor.x, _pos.y * positionCorrectionFactor.y, AudioEngine::instance->listenerPosition.z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerPosition(const glm::vec2& _pos, const float _z)
		{
			AudioEngine::instance->listenerPosition.x = _pos.x;
			AudioEngine::instance->listenerPosition.y = _pos.y;
			AudioEngine::instance->listenerPosition.z = _z;
			alListener3f(AL_POSITION, _pos.x * positionCorrectionFactor.x, _pos.y * positionCorrectionFactor.y, _z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerVelocity(const glm::vec2& _vel)
		{
			AudioEngine::instance->listenerVelocity.x = _vel.x;
			AudioEngine::instance->listenerVelocity.y = _vel.y;
			alListener3f(AL_VELOCITY, _vel.x * positionCorrectionFactor.x, _vel.y * positionCorrectionFactor.y, AudioEngine::instance->listenerVelocity.z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerVelocity(const glm::vec2& _vel, const float _z)
		{
			AudioEngine::instance->listenerVelocity.x = _vel.x;
			AudioEngine::instance->listenerVelocity.y = _vel.y;
			AudioEngine::instance->listenerVelocity.z = _z;
			alListener3f(AL_VELOCITY, _vel.x * positionCorrectionFactor.x, _vel.y * positionCorrectionFactor.y, _z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerGain(const float _gain)
		{
			AudioEngine::instance->listenerGain = _gain;
			alListenerf(AL_GAIN, _gain * applicationData->masterVolume);
		}
		void AudioEngine::setPositionCorrectionFactor(const glm::vec2& _poscor)
		{
			positionCorrectionFactor = _poscor;
			alListener3f(AL_POSITION, AudioEngine::instance->listenerPosition.x * positionCorrectionFactor.x, AudioEngine::instance->listenerPosition.y * positionCorrectionFactor.y, AudioEngine::instance->listenerPosition.z * scaleCorrectionFactor);
			alListener3f(AL_VELOCITY, AudioEngine::instance->listenerVelocity.x * positionCorrectionFactor.x, AudioEngine::instance->listenerVelocity.y * positionCorrectionFactor.y, AudioEngine::instance->listenerVelocity.z * scaleCorrectionFactor);
		}
		void AudioEngine::setScaleCorrectionFactor(const float _sclcor)
		{
			scaleCorrectionFactor = _sclcor;
			alListener3f(AL_POSITION, AudioEngine::instance->listenerPosition.x * positionCorrectionFactor.x, AudioEngine::instance->listenerPosition.y * positionCorrectionFactor.y, AudioEngine::instance->listenerPosition.z * scaleCorrectionFactor);
			alListener3f(AL_VELOCITY, AudioEngine::instance->listenerVelocity.x * positionCorrectionFactor.x, AudioEngine::instance->listenerVelocity.y * positionCorrectionFactor.y, AudioEngine::instance->listenerVelocity.z * scaleCorrectionFactor);
		}
		glm::vec2 AudioEngine::getListenerPosition()
		{
			return glm::vec2(AudioEngine::instance->listenerPosition);
		}
		glm::vec2 AudioEngine::getListenerVelocity()
		{
			return glm::vec2(AudioEngine::instance->listenerVelocity);
		}
		float AudioEngine::getListenerGain()
		{
			return AudioEngine::instance->listenerGain;
		}

		void AudioEngine::updateGain()
		{
			alListenerf(AL_GAIN, AudioEngine::instance->listenerGain * applicationData->masterVolume);
		}


		AudioEngine::AudioEngine() : maxSources(DEFAULT_MAX_SOURCES), listenerGain(1.0f), listenerPosition(0.0f, 0.0f, 1.0f), listenerVelocity(0.0f)
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
						alSourcef(sourcePool[i]->sourceID, AL_ROLLOFF_FACTOR, defaultRollOffFactor);
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
				if (sourcePool[i]->soundPtr->getPriority() >= _soundSource->getPriority())
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