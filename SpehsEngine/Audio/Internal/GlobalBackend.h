#pragma once

#include "soloud/soloud.h"
#include "SpehsEngine/Audio/Types.h"


namespace se
{
	namespace audio
	{
		// SoLoud uses the SoLoud::Soloud object for a lot of things.
		// From here the object is internally accessible to everyone in se::audio.
		// AudioEngine is responsible for creating and deleting the object.
		inline SoLoud::Soloud* globalSoloud = nullptr;

		// Global default values, can be changed by AudioEngine
		inline bool globalDefaultEnableDistanceDelay = false;
		inline float globalDefaultDopplerFactor = 1.0f;
		inline float globalDefaultMinDistance = 1.0f;
		inline float globalDefaultMaxDistance = 100.0f;
		inline DistanceAttenuation globalDefaultDistanceAttenuation = DistanceAttenuation::Linear;
		inline float globalDefaultDistanceAttenuationRolloffFactor = 1.0f;
	}
}
