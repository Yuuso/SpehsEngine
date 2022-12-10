#pragma once

#include "SpehsEngine/Audio/Internal/InternalTypes.h"
#include "SpehsEngine/Audio/Types.h"
#include "SpehsEngine/Audio/Bus.h"


namespace se
{
	namespace audio
	{
		class AudioEngine
		{
		public:

			AudioEngine();
			~AudioEngine();

			AudioEngine(const AudioEngine& _other) = delete;
			AudioEngine& operator=(const AudioEngine& _other) = delete;
			AudioEngine(AudioEngine&& _other) = delete;
			AudioEngine& operator=(AudioEngine&& _other) = delete;


			void update();

			void stopAllSounds();
			void setVoiceLimit(unsigned int _limit);
			void setListenerUp(const glm::vec3& _up);
			void setListenerDirection(const glm::vec3& _direction);
			void setListenerPosition(const glm::vec3& _position);
			void setListenerVelocity(const glm::vec3& _velocity);
			void setAutoVelocityEnabled(bool _enabled);
			void setDistanceDelayEnabled(bool _value);
			void setDefaultDopplerFactor(float _factor);
			void setDefaultAttenuationMinDistance(float _min);
			void setDefaultAttenuationMaxDistance(float _max);
			void setDefaultDistanceAttenuation(DistanceAttenuation _attenuation);
			void setDefaultAttenuationRolloffFactor(float _factor);

			Bus& getMasterBus();
			unsigned int getVoiceLimit() const;
			const glm::vec3& getListenerUp() const;
			const glm::vec3& getListenerDirection() const;
			const glm::vec3& getListenerPosition() const;
			const glm::vec3& getListenerVelocity() const;

		private:

			inline static bool initialized = false;

			struct Listener
			{
				glm::vec3 up{0.0f, 1.0f, 0.0};
				glm::vec3 direction{0.0f, 0.0f, -1.0f};
				glm::vec3 position;
				glm::vec3 velocity;
			};

			std::unique_ptr<Bus> masterBus;
			Listener listener;
			bool autoVelocityEnabled = false;
			glm::vec3 lastListenerPosition;
			time::Time lastUpdate = time::Time::zero;
		};
	}
}
