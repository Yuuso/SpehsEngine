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
	}
}
