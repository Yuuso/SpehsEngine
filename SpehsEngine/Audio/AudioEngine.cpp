#include "stdafx.h"
#include "SpehsEngine/Audio/AudioEngine.h"

#include "SpehsEngine/Audio/Bus.h"
#include "SpehsEngine/Audio/Internal/GlobalBackend.h"
#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Audio/Types.h"


namespace se
{
	namespace audio
	{
		AudioEngine::AudioEngine()
		{
			if (initialized)
			{
				log::fatal("You can only have one AudioEngine!");
				return;
			}
			initialized = true;

			se_assert(!globalSoloud);
			globalSoloud = new SoLoud::Soloud;
			auto result = globalSoloud->init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::BACKENDS::AUTO);
			if (result != 0)
			{
				log::fatal("Failed to initialize AudioEngine, " + std::string(globalSoloud->getErrorString(result)));
				return;
			}

			const char* backendName = globalSoloud->getBackendString();
			if (!backendName)
			{
				log::warning("AudioEngine backend is null!");
			}
			else
			{
				log::info("AudioEngine initialized, " + std::string(backendName));
			}

			setVoiceLimit(128u);
			globalSoloud->setGlobalVolume(1.0f);

			masterBus = std::make_unique<Bus>();
			masterBus->makeMasterBus();
		}
		AudioEngine::~AudioEngine()
		{
			masterBus.reset();

			se_assert(globalSoloud);
			globalSoloud->deinit();
			delete globalSoloud;
			globalSoloud = nullptr;

			se_assert(initialized);
			initialized = false;
		}

		void AudioEngine::update()
		{
			if (autoVelocityEnabled && lastUpdate != time::Time::zero)
			{
				const float deltaSeconds = (time::now() - lastUpdate).asSeconds();
				listener.velocity = (listener.position - lastListenerPosition) / deltaSeconds;
			}
			globalSoloud->set3dListenerParameters(
				listener.position.x,
				listener.position.y,
				listener.position.z,
				listener.direction.x,
				listener.direction.y,
				listener.direction.z,
				listener.up.x,
				listener.up.y,
				listener.up.z,
				listener.velocity.x,
				listener.velocity.y,
				listener.velocity.z
			);
			globalSoloud->update3dAudio();
			lastUpdate = time::now();
			lastListenerPosition = listener.position;
		}

		void AudioEngine::stopAllSounds()
		{
			globalSoloud->stopAll();
		}
		void AudioEngine::setVoiceLimit(unsigned int _limit)
		{
			auto result = globalSoloud->setMaxActiveVoiceCount(_limit);
			if (result != 0)
			{
				log::error("Failed to set max active voice limit to " + std::to_string(_limit) + ", " + std::string(globalSoloud->getErrorString(result)));
				return;
			}
		}
		void AudioEngine::setListenerUp(const glm::vec3& _up)
		{
			listener.up = _up;
		}
		void AudioEngine::setListenerDirection(const glm::vec3& _direction)
		{
			listener.direction = _direction;
		}
		void AudioEngine::setListenerPosition(const glm::vec3& _position)
		{
			listener.position = _position;
		}
		void AudioEngine::setListenerVelocity(const glm::vec3& _velocity)
		{
			listener.velocity = _velocity;
		}
		void AudioEngine::setAutoVelocityEnabled(bool _enabled)
		{
			autoVelocityEnabled = _enabled;
			lastListenerPosition = listener.position;
		}
		void AudioEngine::setDistanceDelayEnabled(bool _value)
		{
			globalDefaultEnableDistanceDelay = _value;
		}
		void AudioEngine::setDefaultDopplerFactor(float _factor)
		{
			globalDefaultDopplerFactor = _factor;
		}
		void AudioEngine::setDefaultAttenuationMinDistance(float _min)
		{
			globalDefaultMinDistance = _min;
		}
		void AudioEngine::setDefaultAttenuationMaxDistance(float _max)
		{
			globalDefaultMaxDistance = _max;
		}
		void AudioEngine::setDefaultDistanceAttenuation(DistanceAttenuation _attenuation)
		{
			globalDefaultDistanceAttenuation = _attenuation;
		}
		void AudioEngine::setDefaultAttenuationRolloffFactor(float _factor)
		{
			globalDefaultDistanceAttenuationRolloffFactor = _factor;
		}

		Bus& AudioEngine::getMasterBus()
		{
			return *masterBus;
		}
		unsigned int AudioEngine::getVoiceLimit() const
		{
			return globalSoloud->getMaxActiveVoiceCount();
		}
		const glm::vec3& AudioEngine::getListenerUp() const
		{
			return listener.up;
		}
		const glm::vec3& AudioEngine::getListenerDirection() const
		{
			return listener.direction;
		}
		const glm::vec3& AudioEngine::getListenerPosition() const
		{
			return listener.position;
		}
		const glm::vec3& AudioEngine::getListenerVelocity() const
		{
			return listener.velocity;
		}
	}
}