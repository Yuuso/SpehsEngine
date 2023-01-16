#pragma once

#include "SpehsEngine/GUI/Units.h"
#include <stdint.h>


namespace se
{
	namespace gui
	{
		typedef int16_t ZIndex;

		template<typename S> using Signal = boost::signals2::signal<S>;
		using Connection = boost::signals2::scoped_connection;

		enum Alignment
		{
			Center		=				0,

			Vertical	=				1 << 0,
			Top			=	 Vertical | 1 << 1,
			Bottom		=	 Vertical | 1 << 2,

			Horizontal	=				1 << 3,
			Left		=  Horizontal | 1 << 4,
			Right		=  Horizontal | 1 << 5,
		};
	}

	namespace legacygui
	{
		typedef gui::ZIndex ZIndex;
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
