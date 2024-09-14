#pragma once


namespace se
{
	namespace audio
	{
		class Bus;
		class AudioResource;
		class AudioSource;
		class AudioEngine;
		struct AudioResourceData;
		typedef AsyncTaskManager<AudioResourceData> ResourceLoader;
		enum class InaudibleBehavior : uint32_t;
		enum class DistanceAttenuation : uint32_t;
	}
}
