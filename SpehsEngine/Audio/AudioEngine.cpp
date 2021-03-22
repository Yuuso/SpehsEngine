#include "stdafx.h"

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/DeltaTimeSystem.h"
#include "SpehsEngine/Audio/AudioEngine.h"
#include "SpehsEngine/Audio/OpenALError.h"
#include "SpehsEngine/Audio/SoundSource.h"

#include <AL\al.h>
#include <AL\alc.h>

#include <algorithm>
#include <iostream>


#define DEFAULT_MAX_SOURCES 255


glm::vec3 positionCorrectionFactor = glm::vec3(1.0f, 1.0f, 1.0f);

extern const float defaultRollOffFactor = 1.0f;


namespace audioVar
{
	std::vector<se::audio::AudioEngine::SourceObject*> sourcePool;
	/*
	default: {0, 0, 1}
	*/
	glm::vec3 listenerPosition;

	/*
	default: {0, 0, 0}
	*/
	glm::vec3 listenerVelocity;

	/*
	default: {0, 0, -1, 0, 1, 0}
	*/
	struct Orientation
	{
		Orientation(){}
		union
		{
			float data[6];
			struct
			{
				glm::vec3 fw;
				glm::vec3 up;
			};
		};
	};
	Orientation listenerOrientation;

	/*
	range: 0.0 -
	default: 1.0

	Each division by 2 equals an attenuation of about -6dB.
	Each multiplicaton by 2 equals an amplification of about +6dB.

	0.0 is silent.
	*/
	float listenerGain;

	/*
	Sound sources can belong to an audio channel.
	Each channel can have their gain level set separately, and the engine automatically updates these changes in the update().
	A negative channel index refers to plain master channel.
	*/
	class AudioChannel
	{
	public:
		/*Enables need to update gain for sound sources, if change is detected*/
		void setGain(const float gain)
		{
			if (_gain == gain)
				return;
			_gain = gain;
			_modified = true;
		}
		/*NOTE: query clears modified state*/
		bool channelModified()
		{
			if (_modified)
			{
				_modified = false;
				return true;
			}
			return false;
		}
		float getGain() const
		{
			return _gain;
		}
	private:
		float _gain = 1.0f;
		bool _modified = false;
	};
	/*
	Audio channels exist in a vector, not visible to outside.
	Channel indices can be referred to without "instantiating a channel"
	*/
	std::vector<AudioChannel> audioChannels;

	size_t maxSources;

	ALCdevice* device;
	ALCcontext* context;

	se::time::DeltaTimeSystem deltaTimeSystem;
}


namespace se
{
	namespace audio
	{
		float AudioEngine::deltaSeconds = 0.0f;
		float AudioEngine::masterVolume = 1.0f;

		void AudioEngine::init()
		{
			audioVar::maxSources = DEFAULT_MAX_SOURCES;
			audioVar::listenerGain = 1.0f;
			audioVar::listenerPosition = glm::vec3(0.0f, 0.0f, 1.0f);
			audioVar::listenerVelocity = glm::vec3(0.0f, 0.0f, 0.0f);
			audioVar::listenerOrientation.up = glm::vec3(0.0f, 1.0f, 0.0f);
			audioVar::listenerOrientation.fw = glm::vec3(0.0f, 0.0f, -1.0f);
			audioVar::deltaTimeSystem.init();

			audioVar::device = alcOpenDevice(NULL);
			if (!audioVar::device)
			{
				log::error("Opening OpenAL device failed!");
			}

			audioVar::context = alcCreateContext(audioVar::device, NULL);
			if (!alcMakeContextCurrent(audioVar::context))
			{
				log::error("Failed to make OpenAL context current!");
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
			audioVar::sourcePool.clear();
			audioVar::device = alcGetContextsDevice(audioVar::context);
			audioVar::audioChannels.clear();
			alcMakeContextCurrent(NULL);
			alcDestroyContext(audioVar::context);
			alcCloseDevice(audioVar::device);
		}

		void AudioEngine::update()
		{
			//Delta time system
			audioVar::deltaTimeSystem.update();
			deltaSeconds = audioVar::deltaTimeSystem.deltaSeconds;

			//Audio channel gain change detection
			for (unsigned c = 0; c < audioVar::audioChannels.size(); c++)
			{
				if (audioVar::audioChannels[c].channelModified())
				{
					//Notify every sound source that belongs to this channel
					for (unsigned s = 0; s < audioVar::sourcePool.size(); s++)
					{
						if (audioVar::sourcePool[s]->soundPtr)
						{
							audioVar::sourcePool[s]->soundPtr->onChannelModified();
						}
					}
				}
			}

			//Source pool
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
			alListener3f(AL_POSITION,
						 audioVar::listenerPosition.x * positionCorrectionFactor.x,
						 audioVar::listenerPosition.y * positionCorrectionFactor.y,
						 audioVar::listenerPosition.z * positionCorrectionFactor.z);
		}
		void AudioEngine::setListenerPosition(const glm::vec3& _pos)
		{
			audioVar::listenerPosition.x = _pos.x;
			audioVar::listenerPosition.y = _pos.y;
			audioVar::listenerPosition.z = _pos.z;
			alListener3f(AL_POSITION,
						 audioVar::listenerPosition.x * positionCorrectionFactor.x,
						 audioVar::listenerPosition.y * positionCorrectionFactor.y,
						 audioVar::listenerPosition.z * positionCorrectionFactor.z);
		}
		void AudioEngine::setListenerVelocity(const glm::vec2& _vel)
		{
			audioVar::listenerVelocity.x = _vel.x;
			audioVar::listenerVelocity.y = _vel.y;
			alListener3f(AL_VELOCITY,
						 audioVar::listenerVelocity.x * positionCorrectionFactor.x,
						 audioVar::listenerVelocity.y * positionCorrectionFactor.y,
						 audioVar::listenerVelocity.z * positionCorrectionFactor.z);
		}
		void AudioEngine::setListenerVelocity(const glm::vec3& _vel)
		{
			audioVar::listenerVelocity.x = _vel.x;
			audioVar::listenerVelocity.y = _vel.y;
			audioVar::listenerVelocity.z = _vel.z;
			alListener3f(AL_VELOCITY,
						 audioVar::listenerVelocity.x * positionCorrectionFactor.x,
						 audioVar::listenerVelocity.y * positionCorrectionFactor.y,
						 audioVar::listenerVelocity.z * positionCorrectionFactor.z);
		}
		void AudioEngine::setListenerUp(const glm::vec3& _up)
		{
			audioVar::listenerOrientation.up = _up;
			alListenerfv(AL_ORIENTATION, audioVar::listenerOrientation.data);
		}
		void AudioEngine::setListenerForward(const glm::vec3& _forward)
		{
			audioVar::listenerOrientation.fw = _forward;
			alListenerfv(AL_ORIENTATION, audioVar::listenerOrientation.data);
		}
		void AudioEngine::setListenerGain(const float _gain)
		{
			audioVar::listenerGain = _gain;
			alListenerf(AL_GAIN, _gain * masterVolume);
		}
		void AudioEngine::setChannelGain(const int _channelIndex, float _gain)
		{
			/*Notify user if creating absurdly large channels quantities*/
#ifdef _DEBUG
			if (_channelIndex > 10000)
				log::warning("Using over 10000 audio channels!");
#endif

			/*Prevent channel gain from going below 0*/
			if (_gain < 0.0f)
				_gain = 0.0f;

			if (_channelIndex < 0)
			{//Default, master channel
				setListenerGain(_gain);
				return;
			}
			else if (_channelIndex >= (int)audioVar::audioChannels.size()) // TODO negative channel index not ok!
			{/*Automatically resize the channel vector if referring to a channel that does not exist yet*/
				audioVar::audioChannels.resize(_channelIndex + 1);
			}

			/*Set channel gain*/
			audioVar::audioChannels[_channelIndex].setGain(_gain);
		}
		float AudioEngine::getChannelGain(const int _channelIndex)
		{
			/*Notify user if creating absurdly large channels quantities*/
#ifdef _DEBUG
			if (_channelIndex > 10000)
				log::warning("Using over 10000 audio channels!");
#endif

			if (_channelIndex < 0)
			{//Default, master channel
				return getListenerGain();
			}
			else if (_channelIndex >= (int)audioVar::audioChannels.size()) // TODO negative channel index not ok!
			{/*Automatically resize the channel vector if referring to a channel that does not exist yet*/
				audioVar::audioChannels.resize(_channelIndex + 1);
			}

			/*Get channel gain*/
			return audioVar::audioChannels[_channelIndex].getGain();
		}
		void AudioEngine::setPositionCorrectionFactor(const glm::vec2& _poscor)
		{
			positionCorrectionFactor.x = _poscor.x;
			positionCorrectionFactor.y = _poscor.y;
			alListener3f(AL_POSITION,
						 audioVar::listenerPosition.x * positionCorrectionFactor.x,
						 audioVar::listenerPosition.y * positionCorrectionFactor.y,
						 audioVar::listenerPosition.z * positionCorrectionFactor.z);
			alListener3f(AL_VELOCITY,
						 audioVar::listenerVelocity.x * positionCorrectionFactor.x,
						 audioVar::listenerVelocity.y * positionCorrectionFactor.y,
						 audioVar::listenerVelocity.z * positionCorrectionFactor.z);
		}
		void AudioEngine::setPositionCorrectionFactor(const glm::vec3& _poscor)
		{
			positionCorrectionFactor = _poscor;
			alListener3f(AL_POSITION,
						 audioVar::listenerPosition.x * positionCorrectionFactor.x,
						 audioVar::listenerPosition.y * positionCorrectionFactor.y,
						 audioVar::listenerPosition.z * positionCorrectionFactor.z);
			alListener3f(AL_VELOCITY,
						 audioVar::listenerVelocity.x * positionCorrectionFactor.x,
						 audioVar::listenerVelocity.y * positionCorrectionFactor.y,
						 audioVar::listenerVelocity.z * positionCorrectionFactor.z);
		}
		glm::vec2 AudioEngine::getListenerPosition()
		{
			return glm::vec2(audioVar::listenerPosition.x, audioVar::listenerPosition.y);
		}
		glm::vec2 AudioEngine::getListenerVelocity()
		{
			return glm::vec2(audioVar::listenerVelocity.x, audioVar::listenerVelocity.y);
		}
		float AudioEngine::getListenerGain()
		{
			return audioVar::listenerGain;
		}

		void AudioEngine::updateGain()
		{
			alListenerf(AL_GAIN, audioVar::listenerGain * masterVolume);
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
				size_t amount = std::min(audioVar::maxSources, audioVar::sourcePool.size() * 2);
				for (size_t i = audioVar::sourcePool.size(); i < amount; i++) //Bit of a messy solution but this isn't called very often so what ever.
					audioVar::sourcePool.push_back(new SourceObject());
				return getFreeSource(_soundSource);
			}

			//If nothing else > steal it from someone with 'lower' priority
			//TODO: Remove unnecessary sorting
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
			log::warning("Could not find a free sound source object!");
			return false;
		}
	}
}