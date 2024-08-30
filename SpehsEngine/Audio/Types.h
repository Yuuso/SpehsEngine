#pragma once


namespace se
{
	namespace audio
	{
		enum class InaudibleBehavior : uint32_t
		{
			Pause,
			Stop,
			Nothing,
		};

		enum class DistanceAttenuation : uint32_t
		{
			Disabled = 0,	// No attenuation, always max volume
			Inverse,		// Very sharp volue drop off
			Linear,			// Linear
			Exponential,	// Closest to natural sound
		};
	}
}
