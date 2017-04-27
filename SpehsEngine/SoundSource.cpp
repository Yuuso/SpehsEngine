
#include "SoundSource.h"

#include "AudioManager.h"
#include "OpenALError.h"
#include "Exceptions.h"
#include "Time.h"

#include <AL\al.h>

#include <algorithm>


extern glm::vec2 positionCorrectionFactor;
extern float scaleCorrectionFactor;

extern const float defaultRollOffFactor;


namespace spehs
{
	namespace audio
	{
		SoundSource::SoundSource() : pitch(1.0f), gain(1.0f), maxGain(1.0f), minGain(0.0f), loop(false), source(nullptr), relativeToSource(true),
			gainAutomationTimer(0.0f), fadeToPause(false), fadeToStop(false), fadeIn(false), playQueued(false), sound(0, 0)
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
		void SoundSource::setSound(const std::string _filepath)
		{
			sound.first = AudioManager::load(_filepath);
			sound.second = 0;
		}
		
		void SoundSource::setParameters()
		{
			alSourcef(source->sourceID, AL_PITCH, pitch);
			alSourcef(source->sourceID, AL_GAIN, gain);
			alSourcef(source->sourceID, AL_MAX_GAIN, maxGain);
			alSourcef(source->sourceID, AL_MIN_GAIN, minGain);
			alSourcei(source->sourceID, AL_LOOPING, loop);
			alSourcei(source->sourceID, AL_SOURCE_RELATIVE, relativeToSource);
			checkOpenALErrors(__FILE__, __LINE__);

			if (sound.second != 0)
				alSourcei(source->sourceID, AL_BUFFER, sound.second);
			checkOpenALErrors(__FILE__, __LINE__);
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
							playQueued = false;
							setParameters();
							alSourcePlay(source->sourceID);
							checkOpenALErrors(__FILE__, __LINE__);
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
				//Automations
				if (gainAutomationTimer > 0.0f)
				{
					float currentGain, currentPitch;
					alGetSourcef(source->sourceID, AL_GAIN, &currentGain);
					alGetSourcef(source->sourceID, AL_PITCH, &currentPitch);

					//Gain
					if (fadeIn)
					{
						float deltaGain = (time::getDeltaTimeAsSeconds() / gainAutomationTimer) * (gain - currentGain);
						alSourcef(source->sourceID, AL_GAIN, std::max(0.0f, currentGain + deltaGain));
						checkOpenALErrors(__FILE__, __LINE__);
					}
					else if (fadeToPause)
					{
						float deltaGain = (time::getDeltaTimeAsSeconds() / gainAutomationTimer) * -currentGain;
						alSourcef(source->sourceID, AL_GAIN, std::max(0.0f, currentGain + deltaGain));
						checkOpenALErrors(__FILE__, __LINE__);
					}
					else if (fadeToStop)
					{
						float deltaGain = (time::getDeltaTimeAsSeconds() / gainAutomationTimer) * -currentGain;
						alSourcef(source->sourceID, AL_GAIN, std::max(0.0f, currentGain + deltaGain));
						checkOpenALErrors(__FILE__, __LINE__);
					}
					else
					{
						if (gain != currentGain)
						{
							float deltaGain = (time::getDeltaTimeAsSeconds() / gainAutomationTimer) * -currentGain;
							alSourcef(source->sourceID, AL_GAIN, std::max(0.0f, currentGain + deltaGain));
							checkOpenALErrors(__FILE__, __LINE__);
						}
					}
					gainAutomationTimer -= time::getDeltaTimeAsSeconds();
				}
				else if (fadeToPause)
				{
					alSourcef(source->sourceID, AL_GAIN, gain);
					pause();
				}
				else if (fadeToStop)
				{
					alSourcef(source->sourceID, AL_GAIN, gain);
					stop();
				}
				else if (fadeIn)
				{
					alSourcef(source->sourceID, AL_GAIN, gain);
					checkOpenALErrors(__FILE__, __LINE__);
					fadeIn = false;
				}
			}
		}

		void SoundSource::play()
		{
			if (sound.second == 0 && !soundQueued())
			{
				spehs::exceptions::warning("Cannot play sound, no sound buffer!");
				return;
			}
			if (!source)
			{
				if (!AudioEngine::getFreeSource(this))
				{
					return;
				}
			}

			resetAutomation();
			if (!soundQueued())
			{
				setParameters();
				alSourcePlay(source->sourceID);
				checkOpenALErrors(__FILE__, __LINE__);
			}
			else
			{
				playQueued = true;
			}
		}
		void SoundSource::play(const float _fadeTimer)
		{
			if (sound.second == 0 && !soundQueued())
			{
				spehs::exceptions::warning("Cannot play sound, no sound buffer!");
				return;
			}
			if (!source)
			{
				if (!AudioEngine::getFreeSource(this))
				{
					return;
				}
			}

			resetAutomation();
			alSourcef(source->sourceID, AL_GAIN, 0.0f);
			gainAutomationTimer = _fadeTimer;
			fadeIn = true;

			if (!soundQueued())
			{
				setParameters();
				alSourcePlay(source->sourceID);
				checkOpenALErrors(__FILE__, __LINE__);
			}
			else
			{
				playQueued = true;
			}
		}
		void SoundSource::pause()
		{
			if (!source)
			{
				spehs::exceptions::warning("Cannot pause sound, no source!");
				return;
			}

			resetAutomation();
			alSourcePause(source->sourceID);
			checkOpenALErrors(__FILE__, __LINE__);
		}
		void SoundSource::pause(const float _fadeTimer)
		{
			if (!source)
			{
				spehs::exceptions::warning("Cannot pause sound, no source!");
				return;
			}

			resetAutomation();
			gainAutomationTimer = _fadeTimer;
			fadeToPause = true;
		}
		void SoundSource::stop()
		{
			if (!source)
			{
				spehs::exceptions::warning("Cannot stop sound, no source!");
				return;
			}

			resetAutomation();
			if (isPlaying())
			{
				alSourceStop(source->sourceID);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}
		void SoundSource::stop(const float _fadeTimer)
		{
			if (!source)
			{
				spehs::exceptions::warning("Cannot stop sound, no source!");
				return;
			}

			resetAutomation();
			gainAutomationTimer = _fadeTimer;
			fadeToStop = true;
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
				alSourcef(source->sourceID, AL_GAIN, gain);
			}
		}
		void SoundSource::setGain(const float _gain, const float _fadeTimer)
		{
			resetAutomation();
			gain = _gain;
			gainAutomationTimer = _fadeTimer;
		}
		void SoundSource::setMaxGain(const float _maxGain)
		{
			maxGain = _maxGain;
			resetAutomation();
			if (source)
			{
				alSourcef(source->sourceID, AL_MAX_GAIN, maxGain);
			}
		}
		void SoundSource::setMinGain(const float _minGain)
		{
			minGain = _minGain;
			if (source)
			{
				alSourcef(source->sourceID, AL_MIN_GAIN, minGain);
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
			fadeToPause = false;
			fadeToStop = false;
			fadeIn = false;
		}
		bool SoundSource::soundQueued()
		{
			//If hashID for sounds exists but there is no buffer, the sound is still loading
			return sound.first != 0 && sound.second == 0;
		}


		ActiveSoundSource::ActiveSoundSource() : SoundSource(), position(0.0f), velocity(0.0f), direction(0.0f), rollOffFactor(defaultRollOffFactor)
		{
			relativeToSource = false;
		}
		ActiveSoundSource::~ActiveSoundSource()
		{

		}

		void ActiveSoundSource::setParameters()
		{
			alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * scaleCorrectionFactor);
			alSource3f(source->sourceID, AL_VELOCITY, velocity.x * positionCorrectionFactor.x, velocity.y * positionCorrectionFactor.y, velocity.z * scaleCorrectionFactor);
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
				alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * scaleCorrectionFactor);
			}
		}
		void ActiveSoundSource::setPosition(const glm::vec2& _pos, const float _z)
		{
			position.x = _pos.x;
			position.y = _pos.y;
			position.y = _z;
			if (source)
			{
				alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * scaleCorrectionFactor);
			}
		}
		void ActiveSoundSource::setVelocity(const glm::vec2& _vel)
		{
			velocity.x = _vel.x;
			velocity.y = _vel.y;
			if (source)
			{
				alSource3f(source->sourceID, AL_VELOCITY, velocity.x * positionCorrectionFactor.x, velocity.y * positionCorrectionFactor.y, velocity.z * scaleCorrectionFactor);
			}
		}
		void ActiveSoundSource::setVelocity(const glm::vec2& _vel, const float _z)
		{
			velocity.x = _vel.x;
			velocity.y = _vel.y;
			velocity.z = _z;
			if (source)
			{
				alSource3f(source->sourceID, AL_VELOCITY, velocity.x * positionCorrectionFactor.x, velocity.y * positionCorrectionFactor.y, velocity.z * scaleCorrectionFactor);
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
				alSource3f(source->sourceID, AL_POSITION, position.x * positionCorrectionFactor.x, position.y * positionCorrectionFactor.y, position.z * scaleCorrectionFactor);
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
