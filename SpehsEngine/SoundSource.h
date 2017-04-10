
#pragma once

#include "AudioEngine.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <utility>


typedef unsigned int ALuint;


namespace spehs
{
	namespace audio
	{
		class SoundSource
		{
			friend class AudioEngine;
		public:
			SoundSource();
			virtual ~SoundSource();

			void setSound(const size_t _hashID);
			void setSound(const std::string _filepath);

			virtual void setParameters();

			void update();

			void play();
			void play(const float _fadeTimer);
			void pause();
			void pause(const float _fadeTimer);
			void stop();
			void stop(const float _fadeTimer);

			void setPitch(const float _pitch);
			virtual void setGain(const float _gain);
			virtual void setGain(const float _gain, const float _fadeTimer);
			void setMaxGain(const float _maxGain);
			void setMinGain(const float _minGain);
			void setLooping(const bool _loop);
			void setPriority(const unsigned int _prio);
			void setRelative(const bool _value);

			bool isPlaying();
			bool isPaused();
			float getPitch(){ return pitch; }
			float getGain(){ return gain; }
			float getMaxGain(){ return maxGain; }
			float getMinGain(){ return minGain; }
			bool getLooping(){ return loop; }
			unsigned int getPriority(){ return priority; }
			bool getRelative(){ return relativeToSource; }

			bool soundQueued();

		protected:
			void removeSource();
			void resetAutomation();

			/*
			range: 0.5 - 2.0
			default: 1.0
			*/
			float pitch;

			/*
			range: 0.0 -
			default: 1.0

			Each division by 2 equals an attenuation of about -6dB.
			Each multiplicaton by 2 equals an amplification of about +6dB.

			0.0 is silent.
			*/
			float gain;

			/*
			range: 0.0 - 1.0
			default: 1.0

			Absolute maximum gain the source will play at.
			*/
			float maxGain;

			/*
			range: 0.0 - 1.0
			default: 0.0

			Absolute minimum gain the source will play at.
			*/
			float minGain;

			/*
			default: false
			*/
			bool loop;

			/*
			range: 0 -
			defaul: 5
			Smaller value means higher priority.
			Value 0 means the source will not be interrupted no matter what.
			*/
			unsigned int priority;

			/*
			range: bool, false
			default: true, false for active sound sources
			Is sound position relative to listener
			Basically, set to false if you want directional sounds
			*/
			bool relativeToSource;

			//Automation
			float gainAutomationTimer;
			bool fadeToPause;
			bool fadeToStop;
			bool fadeIn;
			
			AudioEngine::SourceObject* source;
			std::pair<size_t /*hash*/, ALuint /*buffer*/> sound;
			bool playQueued;
		};

		class ActiveSoundSource : public SoundSource
		{
		public:
			ActiveSoundSource();
			~ActiveSoundSource();

			void setParameters();
			
			void setPosition(const glm::vec2& _pos);
			void setPosition(const glm::vec2& _pos, const float _z);
			void setVelocity(const glm::vec2& _vel);
			void setVelocity(const glm::vec2& _vel, const float _z);
			void setDirection(const glm::vec2& _direction);
			void setZ(const float _z);
			void setRollOffFactor(const float _rollOff);

			glm::vec2 getPosition(){ return glm::vec2(position); }
			glm::vec2 getVelocity(){ return glm::vec2(velocity); }
			glm::vec2 getDirection(){ return direction; }
			float getZ(){ return position.z; }
			float getRollOffFactor(){ return rollOffFactor; }

		private:
			/*
			default: {0, 0, 0}
			*/
			glm::vec3 position;

			/*
			default: {0, 0, 0}
			*/
			glm::vec3 velocity;

			/*
			default: {0, 0}
			*/
			glm::vec2 direction; //Cone stuff to be effective?

			/*
			default: 1.75 (AudioEngine.cpp)
			*/
			float rollOffFactor;
		};
	}
}

