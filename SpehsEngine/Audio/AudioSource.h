#pragma once

#include "boost/signals2/connection.hpp"
#include "glm/vec3.hpp"
#include "SpehsEngine/Audio/AudioResource.h"
#include "SpehsEngine/Audio/Bus.h"
#include "SpehsEngine/Audio/Internal/Instance.h"
#include "SpehsEngine/Audio/Types.h"
#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	namespace audio
	{
		class AudioSource final : public audio::Instance
		{
		public:

			AudioSource();
			~AudioSource();

			AudioSource(const AudioSource& _other) = delete;
			AudioSource& operator=(const AudioSource& _other) = delete;
			AudioSource(AudioSource&& _other) = delete;
			AudioSource& operator=(AudioSource&& _other) = delete;


			void setResource(std::shared_ptr<AudioResource> _resource);
			void setOutput(Bus& _bus);


			/// Play sound in 3D position (restarts from beginning if sound is already playing)
			void play(bool _paused = false);

			/// Play sound in 3D position, other sounds played using playClocked at the same time will be delayed (avoid loads of clumped sounds)
			void playClocked(time::Time _currentTime);

			/// Play sound without 3D positioning
			void playBackground(bool _paused = false);

			/// Set pause state
			void setPause(bool _value);

			/// Stop playing
			void stop();

			/// Seek to a specific time in the sound
			void seek(time::Time _time);

			/// 3D position of the sound source
			void setPosition(const glm::vec3& _position);

			/// 3D velocity of the sound source
			void setVelocity(const glm::vec3& _velocity);

			/// Looping state
			void setLooping(bool _value);

			/// Loop point, 0 by default (start of the sound)
			void setLoopPoint(time::Time _loopPoint);

			/// Protected voices will never be killed. Normally oldest sound will be killed when trying to play more sounds than voice limit.
			void setProtected(bool _value);

			/// Relative play speed, cannot be 0.0
			void setSpeed(float _speed, time::Time _fade = time::Time::zero);

			/// What happens when the sound becomes inaudible
			void setInaudibleBehavior(InaudibleBehavior _behavior);

			/// Min distance for attenuation, max volume used for distances closer
			void setAttenuationMinDistance(float _min);

			/// Max distance for attenuation, farther distances are inaudible
			void setAttenuationMaxDistance(float _max);

			/// Distance attenuation model
			void setDistanceAttenuation(DistanceAttenuation _attenuation);

			/// Rolloff factor for distance attenuation model
			void setAttenuationRolloffFactor(float _factor);

			/// Effectiveness of doppler effect
			void setDopplerFactor(float _factor);


			bool isPlaying() const;
			bool isPaused() const;
			time::Time getStreamPosition() const;
			const glm::vec3& getPosition() const;
			const glm::vec3& getVelocity() const;
			bool getLooping() const;
			time::Time getLoopPoint() const;
			bool getProtected() const;
			float getSpeed() const;
			InaudibleBehavior getInaudibleBehavior() const;
			float getAttenuationMinDistance() const;
			float getAttenuationMaxDistance() const;
			DistanceAttenuation getDistanceAttenuation() const;
			float getAttenuationRolloffFactor() const;
			float getDopplerFactor() const;


		private:

			bool playCommon();
			void applyAttributes() override;
			void applySpeed(time::Time _fade = time::Time::zero);
			void applyInaudibleBehavior();
			void busDestroyedCallback();

			glm::vec3 position;
			glm::vec3 velocity;
			bool looping								= false;
			time::Time loopPoint						= time::Time::zero;
			bool protectedState							= false;
			float speed									= 1.0f;
			InaudibleBehavior inaudibleBehavior			= InaudibleBehavior::Nothing;
			float minDistance							= 0.0f;
			float maxDistance							= 0.0f;
			DistanceAttenuation distanceAttenuation		= DistanceAttenuation::Linear;
			float attenuationRolloff					= 1.0f;
			float dopplerFactor							= 1.0f;

			Bus* outputBus = nullptr;
			boost::signals2::scoped_connection busDestroyedConnection;
			std::shared_ptr<AudioResource> resource;
		};
	}
}
