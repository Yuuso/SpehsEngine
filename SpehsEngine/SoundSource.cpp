
#include "SoundSource.h"

#include "AudioManager.h"
#include "OpenALError.h"
#include "Exceptions.h"

#include <AL\al.h>


extern glm::vec2 positionCorrectionFactor;
extern float scaleCorrectionFactor;

extern const float defaultRollOffFactor;


namespace spehs
{
	namespace audio
	{
		SoundSource::SoundSource() : pitch(1.0f), gain(1.0f), maxGain(1.0f), minGain(0.0f), loop(false), source(nullptr), buffer(0)
		{
		}
		SoundSource::~SoundSource()
		{
			removeSource();
		}

		void SoundSource::setSound(const size_t _hashID)
		{
			buffer = AudioManager::instance->getAudioClip(_hashID).buffer;
			if (source)
			{
				alSourcei(source->sourceID, AL_BUFFER, buffer);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}
		void SoundSource::setSound(const std::string _filepath)
		{
			buffer = AudioManager::instance->getAudioClip(_filepath).buffer;
			if (source)
			{
				alSourcei(source->sourceID, AL_BUFFER, buffer);
				checkOpenALErrors(__FILE__, __LINE__);
			}
		}
		
		void SoundSource::setParameters()
		{
			alSourcef(source->sourceID, AL_PITCH, pitch);
			alSourcef(source->sourceID, AL_GAIN, gain);
			alSourcef(source->sourceID, AL_MAX_GAIN, maxGain);
			alSourcef(source->sourceID, AL_MIN_GAIN, minGain);
			alSourcei(source->sourceID, AL_LOOPING, loop);

			alSourcei(source->sourceID, AL_BUFFER, buffer);
			checkOpenALErrors(__FILE__, __LINE__);
		}

		void SoundSource::play()
		{
			if (buffer == 0)
			{
				spehs::exceptions::warning("Cannot play sound, no sound buffer!");
				return;
			}
			if (!source)
			{
				if (!AudioEngine::instance->getFreeSource(this))
				{
					return;
				}
				setParameters();
			}

			alSourcePlay(source->sourceID);
			checkOpenALErrors(__FILE__, __LINE__);
		}
		void SoundSource::pause()
		{
			if (!source)
			{
				spehs::exceptions::warning("Cannot pause sound, no source!");
				return;
			}

			alSourcePause(source->sourceID);
			checkOpenALErrors(__FILE__, __LINE__);
		}
		void SoundSource::stop()
		{
			if (!source)
			{
				spehs::exceptions::warning("Cannot pause sound, no source!");
				return;
			}

			alSourceStop(source->sourceID);
			checkOpenALErrors(__FILE__, __LINE__);
		}

		void SoundSource::setPitch(const float _pitch)
		{
			pitch = _pitch;
			if (source)
			{
				alSourcef(source->sourceID, AL_PITCH, pitch);
			}
		}
		void SoundSource::setGain(const float _gain)
		{
			gain = _gain;
			if (source)
			{
				alSourcef(source->sourceID, AL_GAIN, gain);
			}
		}
		void SoundSource::setMaxGain(const float _maxGain)
		{
			maxGain = _maxGain;
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


		ActiveSoundSource::ActiveSoundSource() : position(0.0f), velocity(0.0f), direction(0.0f), rollOffFactor(defaultRollOffFactor)
		{
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
