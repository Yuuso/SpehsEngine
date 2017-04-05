
#include "AudioEngine.h"
#include "Exceptions.h"
#include "OpenALError.h"
#include "SoundSource.h"
#include "ApplicationData.h"

#include <AL\al.h>
#include <AL\alc.h>

#include <algorithm>


#define DEFAULT_MAX_SOURCES 256


glm::vec2 positionCorrectionFactor = glm::vec2(1.0f);
float scaleCorrectionFactor = 1.0f;

extern const float defaultRollOffFactor = 1.75f;


namespace audioVar
{
	std::vector<spehs::audio::AudioEngine::SourceObject*> sourcePool;
	/*
	default: {0, 0, 1}
	*/
	glm::vec3 listenerPosition;

	/*
	default: {0, 0, 0}
	*/
	glm::vec3 listenerVelocity;

	/*
	range: 0.0 -
	default: 1.0

	Each division by 2 equals an attenuation of about -6dB.
	Each multiplicaton by 2 equals an amplification of about +6dB.

	0.0 is silent.
	*/
	float listenerGain;

	unsigned int maxSources;

	ALCdevice* device;
	ALCcontext* context;
}


namespace spehs
{
	namespace audio
	{
		void AudioEngine::init()
		{
			audioVar::maxSources = DEFAULT_MAX_SOURCES;
			audioVar::listenerGain = 1.0f;
			audioVar::listenerPosition = glm::vec3(0.0f, 0.0f, 1.0f);
			audioVar::listenerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

			audioVar::device = alcOpenDevice(NULL);
			if (!audioVar::device)
			{
				exceptions::fatalError("Opening OpenAL device failed!");
			}

			audioVar::context = alcCreateContext(audioVar::device, NULL);
			if (!alcMakeContextCurrent(audioVar::context))
			{
				exceptions::fatalError("Failed to make OpenAL context current!");
			}

			checkOpenALCError(audioVar::device, __FILE__, __LINE__);

			for (unsigned i = 0; i < 16; i++) //Bit of a messy solution but this isn't called very often so what ever.
				audioVar::sourcePool.push_back(new SourceObject());
		}
		void AudioEngine::uninit()
		{
			for (unsigned i = 0; i < audioVar::sourcePool.size(); i++)
			{
				delete audioVar::sourcePool[i];
			}
			audioVar::device = alcGetContextsDevice(audioVar::context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(audioVar::context);
			alcCloseDevice(audioVar::device);
		}

		void AudioEngine::update()
		{
			for (unsigned i = 0; i < audioVar::sourcePool.size(); i++)
			{
				if (audioVar::sourcePool[i]->soundPtr)
				{
					audioVar::sourcePool[i]->soundPtr->update();
				}
			}
		}

		void AudioEngine::setMaxSources(const unsigned int _maxSources)
		{
			audioVar::maxSources = _maxSources;
		}

		void AudioEngine::setListenerPosition(const glm::vec2& _pos)
		{
			audioVar::listenerPosition.x = _pos.x;
			audioVar::listenerPosition.y = _pos.y;
			alListener3f(AL_POSITION, _pos.x * positionCorrectionFactor.x, _pos.y * positionCorrectionFactor.y, audioVar::listenerPosition.z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerPosition(const glm::vec2& _pos, const float _z)
		{
			audioVar::listenerPosition.x = _pos.x;
			audioVar::listenerPosition.y = _pos.y;
			audioVar::listenerPosition.z = _z;
			alListener3f(AL_POSITION, _pos.x * positionCorrectionFactor.x, _pos.y * positionCorrectionFactor.y, _z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerVelocity(const glm::vec2& _vel)
		{
			audioVar::listenerVelocity.x = _vel.x;
			audioVar::listenerVelocity.y = _vel.y;
			alListener3f(AL_VELOCITY, _vel.x * positionCorrectionFactor.x, _vel.y * positionCorrectionFactor.y, audioVar::listenerVelocity.z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerVelocity(const glm::vec2& _vel, const float _z)
		{
			audioVar::listenerVelocity.x = _vel.x;
			audioVar::listenerVelocity.y = _vel.y;
			audioVar::listenerVelocity.z = _z;
			alListener3f(AL_VELOCITY, _vel.x * positionCorrectionFactor.x, _vel.y * positionCorrectionFactor.y, _z * scaleCorrectionFactor);
		}
		void AudioEngine::setListenerGain(const float _gain)
		{
			audioVar::listenerGain = _gain;
			alListenerf(AL_GAIN, _gain * applicationData->masterVolume);
		}
		void AudioEngine::setPositionCorrectionFactor(const glm::vec2& _poscor)
		{
			positionCorrectionFactor = _poscor;
			alListener3f(AL_POSITION, audioVar::listenerPosition.x * positionCorrectionFactor.x, audioVar::listenerPosition.y * positionCorrectionFactor.y, audioVar::listenerPosition.z * scaleCorrectionFactor);
			alListener3f(AL_VELOCITY, audioVar::listenerVelocity.x * positionCorrectionFactor.x, audioVar::listenerVelocity.y * positionCorrectionFactor.y, audioVar::listenerVelocity.z * scaleCorrectionFactor);
		}
		void AudioEngine::setScaleCorrectionFactor(const float _sclcor)
		{
			scaleCorrectionFactor = _sclcor;
			alListener3f(AL_POSITION, audioVar::listenerPosition.x * positionCorrectionFactor.x, audioVar::listenerPosition.y * positionCorrectionFactor.y, audioVar::listenerPosition.z * scaleCorrectionFactor);
			alListener3f(AL_VELOCITY, audioVar::listenerVelocity.x * positionCorrectionFactor.x, audioVar::listenerVelocity.y * positionCorrectionFactor.y, audioVar::listenerVelocity.z * scaleCorrectionFactor);
		}
		glm::vec2 AudioEngine::getListenerPosition()
		{
			return glm::vec2(audioVar::listenerPosition);
		}
		glm::vec2 AudioEngine::getListenerVelocity()
		{
			return glm::vec2(audioVar::listenerVelocity);
		}
		float AudioEngine::getListenerGain()
		{
			return audioVar::listenerGain;
		}

		void AudioEngine::updateGain()
		{
			alListenerf(AL_GAIN, audioVar::listenerGain * applicationData->masterVolume);
		}

		bool AudioEngine::getFreeSource(SoundSource* _soundSource)
		{
			for (unsigned i = 0; i < audioVar::sourcePool.size(); i++)
			{
				if (!audioVar::sourcePool[i]->soundPtr)
				{
					if (audioVar::sourcePool[i]->sourceID != 0)
					{
						//Found free source!
						audioVar::sourcePool[i]->soundPtr = _soundSource;
						_soundSource->source = audioVar::sourcePool[i];
						return true;
					}
					else
					{
						//Need to generate source
						alGenSources(1, &audioVar::sourcePool[i]->sourceID);
						checkOpenALErrors(__FILE__, __LINE__);
						alSourcef(audioVar::sourcePool[i]->sourceID, AL_ROLLOFF_FACTOR, defaultRollOffFactor);
						if (audioVar::sourcePool[i]->sourceID != 0)
						{
							audioVar::sourcePool[i]->soundPtr = _soundSource;
							_soundSource->source = audioVar::sourcePool[i];
							return true;
						}
					}
				}
			}

			if (audioVar::sourcePool.size() < audioVar::maxSources)
			{
				//Try making more sources
				int amount = std::min(audioVar::maxSources, audioVar::sourcePool.size() * 2);
				for (unsigned i = audioVar::sourcePool.size(); i < amount; i++) //Bit of a messy solution but this isn't called very often so what ever.
					audioVar::sourcePool.push_back(new SourceObject());
				return getFreeSource(_soundSource);
			}

			//If nothing else > steal it from someone with 'lower' priority
			std::sort(audioVar::sourcePool.begin(), audioVar::sourcePool.end(), [](SourceObject* _a, SourceObject* _b)
			{
				return _a->soundPtr->getPriority() > _b->soundPtr->getPriority();
			});
			for (unsigned i = 0; i < audioVar::sourcePool.size(); i++)
			{
				if (audioVar::sourcePool[i]->soundPtr->getPriority() >= _soundSource->getPriority())
				{
					if (audioVar::sourcePool[i]->soundPtr->getPriority() != 0)
					{
						audioVar::sourcePool[i]->soundPtr->removeSource();
						audioVar::sourcePool[i]->soundPtr = _soundSource;
						_soundSource->source = audioVar::sourcePool[i];
						return true;
					}
				}
			}

			//For now just steal something...
			for (unsigned i = 0; i < audioVar::sourcePool.size(); i++)
			{
				if (audioVar::sourcePool[i]->soundPtr->getPriority() != 0)
				{
					audioVar::sourcePool[i]->soundPtr->removeSource();
					audioVar::sourcePool[i]->soundPtr = _soundSource;
					_soundSource->source = audioVar::sourcePool[i];
					return true;
				}
			}

			//This should be quite rare
			spehs::exceptions::warning("Could not find a free sound source object!");
			return false;
		}
	}
}