
#pragma once

#include <glm/vec2.hpp>

#include <vector>


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
		public:
			static void init();
			static void uninit();

			static void setMaxSources(const unsigned int _maxSources);

			static void setListenerPosition(const glm::vec2& _pos);
			static void setListenerVelocity(const glm::vec2& _vel);
			static void setListenerGain(const float _gain);

			static glm::vec2 getListenerPosition();
			static glm::vec2 getListenerVelocity();
			static float getListenerGain();

		protected:
			static AudioEngine* instance;
			AudioEngine();
			~AudioEngine();
			
			bool getFreeSource(SoundSource* _soundSource);

		private:
			struct SourceObject
			{
				ALuint sourceID = 0;
				SoundSource* soundPtr = nullptr;
			};
			std::vector<SourceObject> sourcePool;
			

			/*
			default: {0, 0}
			*/
			glm::vec2 listenerPosition;

			/*
			default: {0, 0}
			*/
			glm::vec2 listenerVelocity;

			/*
			range: 0.0 -
			default: 1.0

			Each division by 2 equals an attenuation of about -6dB.
			Each multiplicaton by 2 equals an amplification of about +6dB.

			0.0 is silent.
			*/
			float listenerGain;


			unsigned int maxSources;

			ALCdevice* device;
			ALCcontext* context;
		};
	}
}
