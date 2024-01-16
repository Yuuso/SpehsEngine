#include "stdafx.h"
#include "SpehsEngine/Audio/AudioSource.h"

#include "SpehsEngine/Audio/Internal/GlobalBackend.h"


namespace se
{
	namespace audio
	{
		AudioSource::AudioSource()
			: audio::Instance()
			, minDistance(globalDefaultMinDistance)
			, maxDistance(globalDefaultMaxDistance)
			, distanceAttenuation(globalDefaultDistanceAttenuation)
			, attenuationRolloff(globalDefaultDistanceAttenuationRolloffFactor)
			, dopplerFactor(globalDefaultDopplerFactor)
		{}
		AudioSource::~AudioSource()
		{
			stop();
		}

		void AudioSource::setOutput(Bus& _bus)
		{
			outputBus = &_bus;
			if (isHandleValid())
			{
				outputBus->bus->annexSound(handle);
			}
		}
		bool AudioSource::playCommon()
		{
			if (!asset || asset->isLoading() || !asset->isValid())
			{
				log::warning("Cannot play AudioSource, audio asset is invalid or loading!");
				return true;
			}
			if (!outputBus)
			{
				log::warning("Cannot play AudioSource, output bus not set!");
				return true;
			}

			if (isPlaying())
			{
				seek(time::Time::zero);
				return true;
			}

			return false;
		}
		void AudioSource::playBackground(bool _paused)
		{
			if (playCommon())
				return;

			se_assert(!isHandleValid());
			handle = outputBus->bus->play(*asset->getSource(), getVolume(), getPan(), true);
			if (isHandleValid())
			{
				applyAttributes();
				if (!_paused)
				{
					setPause(false);
				}
			}
			else
			{
				log::warning("Failed to play sound! " + asset->getName());
			}
		}
		void AudioSource::playClocked(time::Time _currentTime)
		{
			if (playCommon())
				return;

			se_assert(!isHandleValid());
			handle = outputBus->bus->play3dClocked(
				_currentTime.asSeconds<double>(),
				*asset->getSource(),
				position.x, position.y, position.z,
				velocity.x, velocity.y, velocity.z, getVolume());
			if (isHandleValid())
			{
				applyAttributes();
			}
			else
			{
				log::warning("Failed to play sound! " + asset->getName());
			}
		}
		void AudioSource::play(bool _paused)
		{
			if (playCommon())
				return;

			se_assert(!isHandleValid());
			handle = outputBus->bus->play3d(
				*asset->getSource(),
				position.x, position.y, position.z,
				velocity.x, velocity.y, velocity.z, getVolume(), true);
			if (isHandleValid())
			{
				applyAttributes();
				if (!_paused)
				{
					setPause(false);
				}
			}
			else
			{
				log::warning("Failed to play sound! " + asset->getName());
			}
		}
		void AudioSource::setPause(bool _value)
		{
			if (isHandleValid())
			{
				globalSoloud->setPause(handle, _value);
			}
		}
		void AudioSource::stop()
		{
			if (isHandleValid())
			{
				globalSoloud->stop(handle);
			}
			handle = invalidAudioHandle;
		}
		void AudioSource::seek(time::Time _time)
		{
			if (isHandleValid())
			{
				globalSoloud->seek(handle, _time.asSeconds<double>());
			}
		}

		void AudioSource::setAsset(std::shared_ptr<AudioAsset> _asset)
		{
			stop();
			asset = _asset;
		}
		void AudioSource::setPosition(const glm::vec3& _position)
		{
			position = _position;

			if (isHandleValid())
				globalSoloud->set3dSourcePosition(handle, position.x, position.y, position.z);
		}
		void AudioSource::setVelocity(const glm::vec3& _velocity)
		{
			velocity = _velocity;

			if (isHandleValid())
				globalSoloud->set3dSourceVelocity(handle, velocity.x, velocity.y, velocity.z);
		}
		void AudioSource::setLooping(bool _value)
		{
			looping = _value;

			if (isHandleValid())
				globalSoloud->setLooping(handle, looping);
		}
		void AudioSource::setLoopPoint(time::Time _loopPoint)
		{
			loopPoint = _loopPoint;

			if (isHandleValid())
				globalSoloud->setLoopPoint(handle, loopPoint.asSeconds<double>());
		}
		void AudioSource::setProtected(bool _value)
		{
			protectedState = _value;

			if (isHandleValid())
				globalSoloud->setProtectVoice(handle, protectedState);
		}
		void AudioSource::setSpeed(float _speed, time::Time _fade)
		{
			speed = _speed;

			if (isHandleValid())
			{
				applySpeed(_fade);
			}
			else if (_fade > time::Time::zero)
			{
				log::warning("Cannot fade speed, instance is not valid!");
			}
		}
		void AudioSource::setInaudibleBehavior(InaudibleBehavior _behavior)
		{
			inaudibleBehavior = _behavior;

			if (isHandleValid())
				applyInaudibleBehavior();
		}
		void AudioSource::setAttenuationMinDistance(float _min)
		{
			minDistance = _min;

			if (isHandleValid())
				globalSoloud->set3dSourceMinMaxDistance(handle, minDistance, maxDistance);
		}
		void AudioSource::setAttenuationMaxDistance(float _max)
		{
			maxDistance = _max;

			if (isHandleValid())
				globalSoloud->set3dSourceMinMaxDistance(handle, minDistance, maxDistance);
		}
		void AudioSource::setDistanceAttenuation(DistanceAttenuation _attenuation)
		{
			distanceAttenuation = _attenuation;

			if (isHandleValid())
				globalSoloud->set3dSourceAttenuation(handle, static_cast<unsigned int>(distanceAttenuation), attenuationRolloff);
		}
		void AudioSource::setAttenuationRolloffFactor(float _factor)
		{
			attenuationRolloff = _factor;

			if (isHandleValid())
				globalSoloud->set3dSourceAttenuation(handle, static_cast<unsigned int>(distanceAttenuation), attenuationRolloff);
		}
		void AudioSource::setDopplerFactor(float _factor)
		{
			dopplerFactor = _factor;

			if (isHandleValid())
				globalSoloud->set3dSourceDopplerFactor(handle, dopplerFactor);
		}

		bool AudioSource::isPlaying() const
		{
			return isHandleValid() && !isPaused();
		}
		bool AudioSource::isPaused() const
		{
			if (!isHandleValid())
			{
				return false;
			}
			return globalSoloud->getPause(handle);
		}
		time::Time AudioSource::getStreamPosition() const
		{
			if (!isHandleValid())
			{
				return se::time::Time::zero;
			}
			return time::fromSeconds(globalSoloud->getStreamPosition(handle));
		}
		const glm::vec3& AudioSource::getPosition() const
		{
			return position;
		}
		const glm::vec3& AudioSource::getVelocity() const
		{
			return velocity;
		}
		bool AudioSource::getLooping() const
		{
			return looping;
		}
		time::Time AudioSource::getLoopPoint() const
		{
			return loopPoint;
		}
		bool AudioSource::getProtected() const
		{
			return protectedState;
		}
		float AudioSource::getSpeed() const
		{
			return isHandleValid() ? globalSoloud->getRelativePlaySpeed(handle) : speed;
		}
		InaudibleBehavior AudioSource::getInaudibleBehavior() const
		{
			return inaudibleBehavior;
		}
		float AudioSource::getAttenuationMinDistance() const
		{
			return minDistance;
		}
		float AudioSource::getAttenuationMaxDistance() const
		{
			return maxDistance;
		}
		DistanceAttenuation AudioSource::getDistanceAttenuation() const
		{
			return distanceAttenuation;
		}
		float AudioSource::getAttenuationRolloffFactor() const
		{
			return attenuationRolloff;
		}
		float AudioSource::getDopplerFactor() const
		{
			return dopplerFactor;
		}

		void AudioSource::applyAttributes()
		{
			se_assert(isHandleValid());
			audio::Instance::applyAttributes();
			applySpeed();
			applyInaudibleBehavior();
			globalSoloud->set3dSourcePosition(handle, position.x, position.y, position.z);
			globalSoloud->set3dSourceVelocity(handle, velocity.x, velocity.y, velocity.z);
			globalSoloud->setLooping(handle, looping);
			globalSoloud->setLoopPoint(handle, loopPoint.asSeconds<double>());
			globalSoloud->setProtectVoice(handle, protectedState);
			globalSoloud->set3dSourceMinMaxDistance(handle, minDistance, maxDistance);
			globalSoloud->set3dSourceAttenuation(handle, static_cast<unsigned int>(distanceAttenuation), attenuationRolloff);
			globalSoloud->set3dSourceDopplerFactor(handle, dopplerFactor);
		}
		void AudioSource::applySpeed(time::Time _fade)
		{
			se_assert(isHandleValid());
			if (_fade > time::Time::zero)
			{
				globalSoloud->fadeRelativePlaySpeed(handle, speed, _fade.asSeconds());
			}
			else
			{
				globalSoloud->setRelativePlaySpeed(handle, speed);
			}
		}
		void AudioSource::applyInaudibleBehavior()
		{
			se_assert(isHandleValid());
			switch (inaudibleBehavior)
			{
				default:
					se_assert_m(false, "Invalid InaudibleBehavior!");
					break;
				case InaudibleBehavior::Pause:
					globalSoloud->setInaudibleBehavior(handle, false, false);
					break;
				case InaudibleBehavior::Stop:
					globalSoloud->setInaudibleBehavior(handle, false, true);
					break;
				case InaudibleBehavior::Nothing:
					globalSoloud->setInaudibleBehavior(handle, true, false);
					break;
			}
		}
		void AudioSource::busDestroyedCallback()
		{
			stop();
			outputBus = nullptr;
		}
	}
}
