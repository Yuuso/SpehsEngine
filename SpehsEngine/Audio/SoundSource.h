
#pragma once

#include "SpehsEngine/Audio/AudioEngine.h"
#include "SpehsEngine/Core/Vector.h"

#include <string>
#include <utility>


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
			float pitch;

			/*
			range: 0.0 -
			default: 1.0

			Each division by 2 equals an attenuation of about -6dB.
			Each multiplicaton by 2 equals an amplification of about +6dB.

			Refers to currently set gain. Pausing and stopping should not change the gain setting.

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

			/*
			The sound source gain is affected (multiplied) by the channel's gain level
			*/
			int audioChannel;

			//Automation
			/**Time needed until gain automation is completed*/
			float gainAutomationTimer;
			/**Total seconds to complete the last set gain automation*/
			float gainAutomationTime;
			/**Gain automation begin gain*/
			float gainAutomationBegin;
			/**Gain automation end gain*/
			float gainAutomationEnd;
			/**Automation state*/
			enum class AutomationType : char { none = 0, pause, stop, play } automationType;

			AudioEngine::SourceObject* source;
			std::pair<size_t /*hash*/, ALuint /*buffer*/> sound;
			bool playQueued;
		};

		class ActiveSoundSource : public SoundSource
		{
		public:
			ActiveSoundSource(const int _audioChannel = -1/*by default, no channel*/);
			~ActiveSoundSource();

			void setParameters();
			
			void setPosition(const se::vec2& _pos);
			void setPosition(const se::vec2& _pos, const float _z);
			void setVelocity(const se::vec2& _vel);
			void setVelocity(const se::vec2& _vel, const float _z);
			void setDirection(const se::vec2& _direction);
			void setZ(const float _z);
			void setRollOffFactor(const float _rollOff);

			se::vec2 getPosition(){ return se::vec2(position.x, position.y); }
			se::vec2 getVelocity(){ return se::vec2(velocity.x, velocity.y); }
			se::vec2 getDirection(){ return direction; }
			float getZ(){ return position.z; }
			float getRollOffFactor(){ return rollOffFactor; }

		private:
			/*
			default: {0, 0, 0}
			*/
			se::vec3 position;

			/*
			default: {0, 0, 0}
			*/
			se::vec3 velocity;

			/*
			default: {0, 0}
			*/
			se::vec2 direction; //Cone stuff to be effective?

			/*
			default: 1.75 (AudioEngine.cpp)
			*/
			float rollOffFactor;
		};
	}
}

