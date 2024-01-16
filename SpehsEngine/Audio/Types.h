#pragma once


namespace se
{
	namespace audio
	{
		enum class InaudibleBehavior
		{
			Pause,
			Stop,
			Nothing,
		};

		enum class DistanceAttenuation
		{
			Disabled = 0,	// No attenuation, always max volume
			Inverse,		// Very sharp volume drop off
			Linear,			// Linear
			Exponential,	// Closest to natural sound
		};
	}
}
