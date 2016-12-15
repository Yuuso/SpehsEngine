
#pragma once


struct ALCdevice_struct;
struct ALCcontext_struct;

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;


namespace spehs
{
	namespace audio
	{
		//TODO: General audio playback

		class AudioEngine
		{
		public:
			static AudioEngine* instance;
			static void init();
			static void uninit();

		private:
			AudioEngine();
			~AudioEngine();

			//TODO: Source pool management
			//TODO: priorities and memory management and stuff
			
			//Listener


			//amount of sources at the start & absolute max sources
			//when to increase that number

			//How to snatch sources away from playback

			ALCdevice* device;
			ALCcontext* context;
		};
	}
}
