
#pragma once

#include "SpehsEngine/Audio/AudioEngine.h"

#include <string>
#include <utility>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>


typedef unsigned int ALuint;


namespace se
{
	namespace audio
	{
		class SoundSource
		{
			friend class AudioEngine;
		public:
			SoundSource(const int _audioChannel = -1/*by default, no channel*/);
			virtual ~SoundSource();

			void setSound(const size_t _hashID);
			void setSound(const std::string _filepath);

			virtual void setParameters();

			/**Automatically called by SpehsEngine from AudioEngine::update()*/
			void update();

			/**Automatically called by SpehsEngine when audio channel has been modified*/
			void onChannelModified();

			/*Immediately start playing at the current gain level*/
			void play();
			/*Fade to play from 0 gain level*/
			void play(const float _fadeTimer);
			/*Immediately pause playing*/
			void pause();
			/*Fade to pause from current gain level*/
			void pause(const float _fadeTimer);
			/*Immediately stop playing*/
			void stop();
			/*Fade to stop from current gain level*/
			void stop(const float _fadeTimer);

			void setPitch(const float _pitch);
			virtual void setGain(const float _gain);
			virtual void setGain(const float _gain, const float _fadeTimer);
			void setMaxGain(const float _maxGain);
			void setMinGain(const float _minGain);
			void setLooping(const bool _loop);
			void setPriority(const unsigned int _prio);
			void setRelative(const bool _value);
			void setAudioChannel(const int _audioChannel);

			bool isPlaying();
			bool isPaused();
			float getPitch(){ return pitch; }
			float getGain(){ return gain; }
			float getMaxGain(){ return maxGain; }
			float getMinGain(){ return minGain; }
			bool getLooping(){ return loop; }
			unsigned int getPriority(){ return priority; }
			bool getRelative(){ return relativeToSource; }
			size_t getSoundHash() const { return sound.first; }
			int getAudioChannel() const { return audioChannel; }

			bool soundQueued();

		protected:
			void removeSource();
			void resetAutomation();

			/*
			range: 0.5 - 2.0
			default: 1.0
			*/
			float pitch = 1.0f;

			/*
			range: 0.0 -
			default: 1.0

			Each division by 2 equals an attenuation of about -6dB.
			Each multiplicaton by 2 equals an amplification of about +6dB.

			Refers to currently set gain. Pausing and stopping should not change the gain setting.

			0.0 is silent.
			*/
			float gain = 1.0f;

			/*
			range: 0.0 - 1.0
			default: 1.0

			Absolute maximum gain the source will play at.
			*/
			float maxGain = 1.0f;

			/*
			range: 0.0 - 1.0
			default: 0.0

			Absolute minimum gain the source will play at.
			*/
			float minGain = 0.0f;

			/*
			default: false
			*/
			bool loop = false;

			/*
			range: 0 -
			defaul: 5
			Smaller value means higher priority.
			Value 0 means the source will not be interrupted no matter what.
			*/
			unsigned int priority = 5;

			/*
			range: bool, false
			default: true, false for active sound sources
			Is sound position relative to listener
			Basically, set to false if you want directional sounds
			*/
			bool relativeToSource = true;

			/*
			The sound source gain is affected (multiplied) by the channel's gain level
			*/
			int audioChannel = 0;

			//Automation
			/**Time needed until gain automation is completed*/
			float gainAutomationTimer = 0.0f;
			/**Total seconds to complete the last set gain automation*/
			float gainAutomationTime = 0.0f;
			/**Gain automation begin gain*/
			float gainAutomationBegin = 0.0f;
			/**Gain automation end gain*/
			float gainAutomationEnd = 0.0f;
			/**Automation state*/
			enum class AutomationType : char { none = 0, pause, stop, play } automationType = AutomationType::none;

			AudioEngine::SourceObject* source = nullptr;
			std::pair<size_t /*hash*/, ALuint /*buffer*/> sound = {(size_t)0, (ALuint)0};
			bool playQueued = false;
		};

		class ActiveSoundSource : public SoundSource
		{
		public:
			ActiveSoundSource(const int _audioChannel = -1/*by default, no channel*/);
			~ActiveSoundSource();

			void setParameters();
			
			void setPosition(const glm::vec2& _pos);
			void setPosition(const glm::vec2& _pos, const float _z);
			void setVelocity(const glm::vec2& _vel);
			void setVelocity(const glm::vec2& _vel, const float _z);
			void setDirection(const glm::vec2& _direction);
			void setZ(const float _z);
			void setRollOffFactor(const float _rollOff);

			glm::vec2 getPosition(){ return glm::vec2(position.x, position.y); }
			glm::vec2 getVelocity(){ return glm::vec2(velocity.x, velocity.y); }
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
			float rollOffFactor = 1.75f;
		};
	}
}

