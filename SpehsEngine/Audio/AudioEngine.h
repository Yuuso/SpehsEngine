
#pragma once

#include <vector>

#include "SpehsEngine/Core/Vector.h"


struct ALCdevice_struct;
struct ALCcontext_struct;

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

typedef unsigned int ALuint;


namespace spehs
{
	namespace audio
	{
		//easy debug/temp sounds

		class AudioEngine
		{
			friend class SoundSource;
			friend class ApplicationData;
			friend class ActiveSoundSource;
		public:
			struct SourceObject
			{
				ALuint sourceID = 0;
				SoundSource* soundPtr = nullptr;
			};

			static void init();
			static void uninit();

			static void update();

			static void setMaxSources(const unsigned int _maxSources);

			static void setListenerPosition(const spehs::vec2& _pos);
			static void setListenerPosition(const spehs::vec2& _pos, const float _z);
			static void setListenerVelocity(const spehs::vec2& _vel);
			static void setListenerVelocity(const spehs::vec2& _vel, const float _z);
			static void setListenerGain(const float _gain);

			//These correction factors are applied to all sound and the listener positions and velocities. They are used to set the scale of the units used in positions.
			static void setPositionCorrectionFactor(const spehs::vec2& _poscor);
			static void setScaleCorrectionFactor(const float _sclcor); //z

			static spehs::vec2 getListenerPosition();
			static spehs::vec2 getListenerVelocity();
			static float getListenerGain();

			//Audio channels
			/**If referencing previously non-existing channel, channel is created. A negative channel index refers to plain master channel.*/
			static void setChannelGain(const int _channelIndex, float _gain);
			/**If referencing previously non-existing channel, channel is created. A negative channel index refers to plain master channel.*/
			static float getChannelGain(const int _channelIndex);

			static float deltaSeconds;

		protected:			
			static void updateGain(); //Call when master volume has changed
			static bool getFreeSource(SoundSource* _soundSource);
		};
	}
}
