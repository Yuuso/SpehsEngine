#pragma once

#include "stdafx.h"


namespace se::gui
{
	enum class EndBehavior : uint8_t
	{
		Stop,				// Stop animation
		Hold,				// Hold last value
		ReverseOnce,		// Plays in reverse, then Stop
		Repeat,				// Starts from the beginning, then repeats forever
		ReverseRepeat,		// Plays back to start in reverse, then repeats forever
	};

	enum class EasingMode : uint8_t
	{
		EaseIn,				// Normal
		EaseOut,			// Inverse
		EaseInOut			// Both
	};

	enum class EasingFunction : uint8_t
	{
		Discrete,
		Linear,
		Sine,
		Back,
		Bounce,
		Elastic,
		Quadratic,
		Cubic,
		Quartic,
		Quintic,
		Exponential,
		Circle,
	};

	template<typename Type>
	struct KeyFrame
	{
		time::Time timePoint = time::Time::zero;
		Type value;
		EasingFunction easingFunction = EasingFunction::Linear;
		EasingMode easingMode = EasingMode::Normal;
	};

	template<typename Type>
	class Animation
	{
	public:
		std::string targetProperty;
		std::string targetElement;
		std::vector<KeyFrame<Type>> keyFrames;
		EndBehavior endBehavior = EndBehavior::Stop;
	};

	class AnimationAction
	{};
	class AnimationGroupAction
	{};
	// stop
	// pause
}
