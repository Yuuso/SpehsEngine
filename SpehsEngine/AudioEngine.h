
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

			ALCdevice* device;
			ALCcontext* context;
		};
	}
}
