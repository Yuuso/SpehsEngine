
#pragma once

#include "AudioEngine.h"

#include <glm/vec2.hpp>

#include <string>


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
			~SoundSource();

			void setSound(const size_t _hashID);
			void setSound(const std::string _filepath);

			void play();
			void pause();
			void stop();

			void setPosition(const glm::vec2& _pos);
			void setVelocity(const glm::vec2& _vel);
			void setDirection(const glm::vec2& _direction);
			void setPitch(const float _pitch);
			void setGain(const float _gain);
			void setMaxGain(const float _maxGain);
			void setMinGain(const float _minGain);
			void setLooping(const bool _loop);
			void setPriority(const unsigned int _prio);

			bool isPlaying();
			bool isPaused();
			glm::vec2 getPosition(){ return position; }
			glm::vec2 getVelocity(){ return velocity; }
			glm::vec2 getDirection(){ return direction; }
			float getPitch(){ return pitch; }
			float getGain(){ return gain; }
			float getMaxGain(){ return maxGain; }
			float getMinGain(){ return minGain; }
			bool getLooping(){ return loop; }
			unsigned int getPriority(){ return priority; }

		private:

			void removeSource();

			/*
			default: {0, 0, 0}
			*/
			glm::vec2 position;
			
			/*
			default: {0, 0, 0}
			*/
			glm::vec2 velocity;
			
			/*
			default: {0, 0, 0}
			*/
			glm::vec2 direction; //Cone stuff to be effective?

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

			AudioEngine::SourceObject* source;
			ALuint buffer;
		};
	}
}

