#pragma once

#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Core/Color.h"


namespace se
{
	namespace graphics
	{
		struct TextStyle
		{
			bool operator==(const TextStyle& _other)
			{
				return color == _other.color;
			}

			se::Color color;

			// font index
			// scale
			// background color
			// strikethrough color
			// underline color
			// border color
			// offset
			// kerning
		};

		struct StyledStringSegment
		{
			TextStyle style;
			std::basic_string<CharacterCode> text; // utf-32
		};

		typedef std::vector<std::unique_ptr<StyledStringSegment>> StyledString;
	}
}
