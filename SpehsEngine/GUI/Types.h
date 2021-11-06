#pragma once

#include "SpehsEngine/GUI/Units.h"
#include <stdint.h>


namespace se
{
	namespace gui
	{
		typedef int16_t ZIndex;

		typedef uint16_t GUIElementUpdateFlagsType;

		struct Margin
		{
			Margin() = default;
			Margin(GUIUnit _value)
				: left(_value), top(_value), right(_value), bottom(_value) {}
			Margin(GUIUnit _left, GUIUnit _top, GUIUnit _right, GUIUnit _bottom)
				: left(_left), top(_top), right(_right), bottom(_bottom) {}

			GUIUnit left;
			GUIUnit top;
			GUIUnit right;
			GUIUnit bottom;
		};
		typedef Margin Padding;

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
	}
}
