#pragma once

#include "SpehsEngine/GUI/Units.h"
#include <stdint.h>


namespace se
{
	namespace gui
	{
		typedef int16_t ZIndex;

		typedef uint16_t GUIElementUpdateFlagsType;

		enum class VerticalAlignment
		{
			Top,
			Center,
			Middle = Center,
			Bottom
		};
		enum class HorizontalAlignment
		{
			Left,
			Center,
			Middle = Center,
			Right
		};

		typedef VerticalAlignment VerticalAnchor;
		typedef HorizontalAlignment HorizontalAnchor;

		enum class StackOrientation
		{
			Vertical,
			Horizontal
		};

		enum class LayerMaskStyle
		{
			/*
			* Elements are masked with a global layer counter.
			* This means that you can use almost as many layer masked elements as you want.
			* Masked elements can bleed to other nearby masked elements that are on the same layer.
			* There is a max limit of 255 nested masked elements.
			*
			* Use this only if you have a ton of masked elements and the limit from 'Incrementing' style is not enough.
			*/
			Depth,

			/*
			* Similar to 'Depth', but elements without a masking parent start from a new layer.
			* Child elements who have the same masking parent however still share the same masking layer.
			* There is a limit of 255 total unique masked elements.
			*
			* Use this as a default.
			*/
			Incrementing,

			/*
			* Masked elements reserve one bit for masking.
			* This means there is never bleeding to neighboring elements.
			* But masked elements are limited to only 8!
			*
			* Use this if you don't need more than 8 total masked elements in the view.
			*/
			BitMask,
		};
	}
}
