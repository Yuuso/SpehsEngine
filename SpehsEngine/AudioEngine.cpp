
#include "AudioEngine.h"
#include "Exceptions.h"
#include "OpenALError.h"

#include <AL\al.h>
#include <AL\alc.h>


namespace spehs
{
	namespace audio
	{
		AudioEngine* AudioEngine::instance = nullptr;
		void AudioEngine::init()
		{
			if (!AudioEngine::instance)
			{
				AudioEngine::instance = new AudioEngine;
			}
			else
			{
				exceptions::fatalError("Audio engine already initialized!");
			}
		}
		void AudioEngine::uninit()
		{
			if (AudioEngine::instance)
			{
				delete AudioEngine::instance;
			}
			else
			{
				exceptions::warning("Cannot uninit audio engine, instance not initialized!");
			}
		}

		AudioEngine::AudioEngine()
		{
			device = alcOpenDevice(NULL);
			if (!device)
			{
				exceptions::fatalError("Opening OpenAL device failed!");
			}

			context = alcCreateContext(device, NULL);
			if (!alcMakeContextCurrent(context))
			{
				exceptions::fatalError("Failed to make OpenAL context current!");
			}

			checkOpenALCError(device, __FILE__, __LINE__);
		}
		AudioEngine::~AudioEngine()
		{
			device = alcGetContextsDevice(context);
			alcMakeContextCurrent(NULL);
			alcDestroyContext(context);
			alcCloseDevice(device);
		}
	}
}