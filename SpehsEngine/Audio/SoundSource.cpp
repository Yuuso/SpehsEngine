#include "stdafx.h"

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Audio/SoundSource.h"
#include "SpehsEngine/Audio/AudioManager.h"
#include "SpehsEngine/Audio/AudioEngine.h"
#include "SpehsEngine/Audio/OpenALError.h"

#include <AL\al.h>

#include <algorithm>


extern glm::vec3 positionCorrectionFactor;

extern const float defaultRollOffFactor;


namespace se
{
	namespace audio
	{
		SoundSource::SoundSource(const int _audioChannel)
			: audioChannel(_audioChannel)
		{
		}

		SoundSource::~SoundSource()
		{
			removeSource();
		}

		void SoundSource::setSound(const size_t _hashID)
		{
			sound.first = _hashID;
			sound.second = AudioManager::isReady(_hashID);
		}

		void SoundSource::setSound(const std::string& _filepath)
		{
			sound.first = AudioManager::load(_filepath);
			sound.second = 0;
		}

		void SoundSource::setParameters()
		{
			if (source)
			{
				const float channelGain = AudioEngine::getChannelGain(audioChannel);

				alSourcef(source->sourceID, AL_PITCH, pitch);
				alSourcef(source->sourceID, AL_GAIN, gain * channelGain);
				alSourcef(source->sourceID, AL_MAX_GAIN, maxGain * channelGain);
				alSourcef(source->sourceID, AL_MIN_GAIN, minGain * channelGain);
				alSourcei(source->sourceID, AL_LOOPING, loop);
				alSourcei(source->sourceID, AL_SOURCE_RELATIVE, relativeToSource);
				checkOpenALErrors(__FILE__, __LINE__);

				if (sound.second != 0 && !isPlaying())
					alSourcei(source->sourceID, AL_BUFFER, sound.second);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}

		void SoundSource::update()
		{
			if (!isPlaying())
			{
				if (!playQueued)
				{
					//This too brutal?
					removeSource();
				}
				if (soundQueued())
				{
					sound.second = AudioManager::isReady(sound.first);
					if (sound.second != 0)
					{
						if (playQueued)
						{
							if (source ||
								(!source && AudioEngine::getFreeSource(this)))
							{
								setParameters();
								alSourcePlay(source->sourceID);
								checkOpenALErrors(__FILE__, __LINE__);
							}
							playQueued = false;
						}
					}
				}
			}

			if (!soundQueued())
			{
				if (!source)
				{
					resetAutomation();
				}

				//Gain automation
				if (automationType != AutomationType::none)
				{
					/*State of automation on a scale from 0.0 to 1.0*/
					const float automationState = (gainAutomationTime - gainAutomationTimer) / gainAutomationTime;

					/*Linearly fade gain*/
					const float automationGain = (1.0f - automationState) * gainAutomationBegin + automationState * gainAutomationEnd;

					/*Set calculated gain*/
					alSourcef(source->sourceID, AL_GAIN, automationGain);
					checkOpenALErrors(__FILE__, __LINE__);

					/*Decrement automation timer*/
					gainAutomationTimer -= AudioEngine::deltaSeconds;

					/*Check end of automation*/
					if (gainAutomationTimer <= 0.0f)
					{//End of automation

						/*Take an end action*/
						switch (automationType)
						{
						case AutomationType::pause:
							alSourcef(source->sourceID, AL_GAIN, 0.0f);
							pause();
							break;
						case AutomationType::stop:
							alSourcef(source->sourceID, AL_GAIN, 0.0f);
							stop();
							break;
						case AutomationType::play:
							alSourcef(source->sourceID, AL_GAIN, gain);
							checkOpenALErrors(__FILE__, __LINE__);
							break;
						default:
							log::error(__FUNCTION__": Invalid automation type!");
							break;
						}
						automationType = AutomationType::none;
					}
				}
			}
		}

		void SoundSource::onChannelModified()
		{//Audio channel was modified since last update, called from AudioEngine::update()
			setParameters();
		}

		void SoundSource::play()
		{
			resetAutomation();
			if (sound.second == 0 && !soundQueued())
			{
				log::warning("Cannot play sound, no sound buffer!");
				return;
			}
			if (!source)
			{
				if (!AudioEngine::getFreeSource(this))
				{
					return;
				}
			}

			if (soundQueued())
			{
				playQueued = true;
			}
			else
			{
				setParameters();
				alSourcePlay(source->sourceID);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}

		void SoundSource::play(const float _fadeTimer)
		{
			resetAutomation();
			if (sound.second == 0 && !soundQueued())
			{
				log::warning("Cannot play sound, no sound buffer!");
				return;
			}
			if (!source)
			{
				if (!AudioEngine::getFreeSource(this))
				{
					return;
				}
			}

			/*Set type to play*/
			automationType = AutomationType::play;
			/*Set begin and end gain levels*/
			gainAutomationBegin = 0.0f;
			gainAutomationEnd = gain;
			/*Set automation timers*/
			gainAutomationTime = _fadeTimer;
			gainAutomationTimer = _fadeTimer;

			if (soundQueued())
			{
				playQueued = true;
			}
			else
			{
				setParameters();
				alSourcePlay(source->sourceID);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}

		void SoundSource::pause()
		{
			resetAutomation();
			if (!source)
			{
				log::warning("Cannot pause sound, no source!");
				return;
			}

			alSourcePause(source->sourceID);
			checkOpenALErrors(__FILE__, __LINE__);
		}

		void SoundSource::pause(const float _fadeTimer)
		{
			resetAutomation();
			if (!source)
			{
				log::warning("Cannot pause sound, no source!");
				return;
			}

			/*Set type to pause*/
			automationType = AutomationType::pause;
			/*Set begin and end gain levels*/
			gainAutomationBegin = gain;
			gainAutomationEnd = 0.0f;
			/*Set automation timers*/
			gainAutomationTime = _fadeTimer;
			gainAutomationTimer = _fadeTimer;
		}

		void SoundSource::stop()
		{
			resetAutomation();
			if (!source)
			{
				log::warning("Cannot stop sound, no source!");
				return;
			}

			if (isPlaying())
			{
				alSourceStop(source->sourceID);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}

		void SoundSource::stop(const float _fadeTimer)
		{
			resetAutomation();
			if (!source)
			{
				log::warning("Cannot stop sound, no source!");
				return;
			}

			/*Set type to stop*/
			automationType = AutomationType::stop;
			/*Set begin and end gain levels*/
			gainAutomationBegin = gain;
			gainAutomationEnd = 0.0f;
			/*Set automation timers*/
			gainAutomationTime = _fadeTimer;
			gainAutomationTimer = _fadeTimer;
		}

		void SoundSource::setPitch(const float _pitch)
		{
			pitch = _pitch;
			resetAutomation();
			if (source)
			{
				alSourcef(source->sourceID, AL_PITCH, pitch);
			}
		}

		void SoundSource::setGain(const float _gain)
		{
			gain = _gain;
			resetAutomation();
			if (source)
			{
				alSourcef(source->sourceID, AL_GAIN, gain * AudioEngine::getChannelGain(audioChannel));
			}
		}

		void SoundSource::setGain(const float _gain, const float _fadeTimer)
		{
			/*Set type to play*/
			automationType = AutomationType::play;
			/*Set begin and end gain levels*/
			gainAutomationBegin = gain;
			gainAutomationEnd = _gain;
			/*Set automation timers*/
			gainAutomationTime = _fadeTimer;
			gainAutomationTimer = _fadeTimer;
		}

		void SoundSource::setMaxGain(const float _maxGain)
		{
			maxGain = _maxGain;
			if (source)
			{
				alSourcef(source->sourceID, AL_MAX_GAIN, maxGain * AudioEngine::getChannelGain(audioChannel));
			}
		}

		void SoundSource::setMinGain(const float _minGain)
		{
			minGain = _minGain;
			if (source)
			{
				alSourcef(source->sourceID, AL_MIN_GAIN, minGain * AudioEngine::getChannelGain(audioChannel));
			}
		}

		void SoundSource::setLooping(const bool _loop)
		{
			loop = _loop;
			if (source)
			{
				alSourcei(source->sourceID, AL_LOOPING, loop);
			}
		}

		void SoundSource::setPriority(const unsigned int _prio)
		{
			priority = _prio;
		}

		void SoundSource::setRelative(const bool _value)
		{
			relativeToSource = _value;
		}

		void SoundSource::setAudioChannel(const int _audioChannel)
		{
			if (audioChannel != _audioChannel)
			{
				audioChannel = _audioChannel;
				/*Channel changed, update parameters*/
				setParameters();
			}
		}

		bool SoundSource::isPlaying()
		{
			if (!source)
			{
				return false;
			}
			ALint state;
			alGetSourcei(source->sourceID, AL_SOURCE_STATE, &state);
			return state == AL_PLAYING;
		}

		bool SoundSource::isPaused()
		{
			if (!source)
			{
				return false;
			}
			ALint state;
			alGetSourcei(source->sourceID, AL_SOURCE_STATE, &state);
			return state == AL_PAUSED;
		}

		void SoundSource::removeSource()
		{
			if (!source)
			{
				return;
			}
			stop();
			source->soundPtr = nullptr;
			source = nullptr;
		}

		void SoundSource::resetAutomation()
		{
			gainAutomationTimer = 0.0f;
			automationType = AutomationType::none;
		}

		bool SoundSource::soundQueued()
		{
			//If hashID for sounds exists but there is no buffer, the sound is still loading
			return sound.first != 0 && sound.second == 0;
		}



		ActiveSoundSource::ActiveSoundSource(const int _audioChannel)
			: SoundSource(_audioChannel)
			, rollOffFactor(defaultRollOffFactor)
		{
			relativeToSource = false;
		}

		ActiveSoundSource::~ActiveSoundSource()
		{

		}

		void ActiveSoundSource::setParameters()
		{
			alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * positionCorrectionFactor.z);
			alSource3f(source->sourceID, AL_VELOCITY, velocity.x * positionCorrectionFactor.x, velocity.y * positionCorrectionFactor.y, velocity.z * positionCorrectionFactor.z);
			alSource3f(source->sourceID, AL_DIRECTION, direction.x, direction.y, 0.0f);
			alSourcef(source->sourceID, AL_ROLLOFF_FACTOR, rollOffFactor);
			SoundSource::setParameters();
		}

		void ActiveSoundSource::setPosition(const glm::vec2& _pos)
		{
			position.x = _pos.x;
			position.y = _pos.y;
			if (source)
			{
				alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * positionCorrectionFactor.z);
			}
		}

		void ActiveSoundSource::setPosition(const glm::vec2& _pos, const float _z)
		{
			position.x = _pos.x;
			position.y = _pos.y;
			position.y = _z;
			if (source)
			{
				alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * positionCorrectionFactor.z);
			}
		}

		void ActiveSoundSource::setVelocity(const glm::vec2& _vel)
		{
			velocity.x = _vel.x;
			velocity.y = _vel.y;
			if (source)
			{
				alSource3f(source->sourceID, AL_VELOCITY, velocity.x * positionCorrectionFactor.x, velocity.y * positionCorrectionFactor.y, velocity.z * positionCorrectionFactor.z);
			}
		}

		void ActiveSoundSource::setVelocity(const glm::vec2& _vel, const float _z)
		{
			velocity.x = _vel.x;
			velocity.y = _vel.y;
			velocity.z = _z;
			if (source)
			{
				alSource3f(source->sourceID, AL_VELOCITY, velocity.x * positionCorrectionFactor.x, velocity.y * positionCorrectionFactor.y, velocity.z * positionCorrectionFactor.z);
			}
		}

		void ActiveSoundSource::setDirection(const glm::vec2& _direction)
		{
			direction = _direction;
			if (source)
			{
				alSource3f(source->sourceID, AL_DIRECTION, direction.x, direction.y, 0.0f);
			}
		}

		void ActiveSoundSource::setZ(const float _z)
		{
			position.z = _z;
			if (source)
			{
				alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * positionCorrectionFactor.z);
			}
		}

		void ActiveSoundSource::setRollOffFactor(const float _rollOff)
		{
			rollOffFactor = _rollOff;
			if (source)
			{
				alSourcef(source->sourceID, AL_ROLLOFF_FACTOR, rollOffFactor);
			}
		}
	}
}
