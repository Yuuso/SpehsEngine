
#include "SoundSource.h"

#include "AudioManager.h"
#include "Console.h"

#include <AL\al.h>


namespace spehs
{
	namespace audio
	{
		SoundSource::SoundSource() : position(0.0f), velocity(0.0f), direction(0.0f), pitch(1.0f), gain(1.0f), maxGain(1.0f), minGain(0.0f), loop(false), source(0), buffer(0)
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
			}
		}
		void SoundSource::setSound(const std::string _filepath)
		{
			buffer = AudioManager::instance->getAudioClip(_filepath).buffer;
		}

		void SoundSource::play()
		{
			if (buffer == 0)
			{
				spehs::console::warning("Cannot play sound, no sound buffer!");
				return;
			}
			if (!source)
			{
				if (!AudioEngine::instance->getFreeSource(this))
				{
					return;
				}

				alSource3f(source->sourceID, AL_POSITION, position.x, position.y, 0.0f);
				alSource3f(source->sourceID, AL_VELOCITY, velocity.x, velocity.y, 0.0f);
				alSource3f(source->sourceID, AL_DIRECTION, direction.x, direction.y, 0.0f);
				alSourcef(source->sourceID, AL_PITCH, pitch);
				alSourcef(source->sourceID, AL_GAIN, gain);
				alSourcef(source->sourceID, AL_MAX_GAIN, maxGain);
				alSourcef(source->sourceID, AL_MIN_GAIN, minGain);
				alSourcei(source->sourceID, AL_LOOPING, loop);

				alSourcei(source->sourceID, AL_BUFFER, buffer);
			}

			alSourcePlay(source->sourceID);
		}
		void SoundSource::pause()
		{
			if (!source)
			{
				spehs::console::warning("Cannot pause sound, no source!");
				return;
			}

			alSourcePause(source->sourceID);
		}
		void SoundSource::stop()
		{
			if (!source)
			{
				spehs::console::warning("Cannot pause sound, no source!");
				return;
			}

			alSourceStop(source->sourceID);
		}

		void SoundSource::setPosition(const glm::vec2& _pos)
		{
			position = _pos;
			if (source)
			{
				alSource3f(source->sourceID, AL_POSITION, position.x, position.y, 0.0f);
			}
		}
		void SoundSource::setVelocity(const glm::vec2& _vel)
		{
			velocity = _vel;
			if (source)
			{
				alSource3f(source->sourceID, AL_VELOCITY, velocity.x, velocity.y, 0.0f);
			}
		}
		void SoundSource::setDirection(const glm::vec2& _direction)
		{
			direction = _direction;
			if (source)
			{
				alSource3f(source->sourceID, AL_DIRECTION, direction.x, direction.y, 0.0f);
			}
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
	}
}
