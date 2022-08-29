#pragma once


namespace SoLoud
{
	class AudioSource;
	class Bus;
	class Queue;
}

namespace se
{
	namespace audio
	{
		typedef unsigned int AudioHandle;
		typedef SoLoud::AudioSource AudioSourceBase;
		typedef SoLoud::Bus AudioBus;

		static constexpr AudioHandle invalidAudioHandle = 0;
	}
}
