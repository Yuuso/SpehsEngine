#pragma once

#include "SpehsEngine/Graphics/CharacterMap.h"
#include "SpehsEngine/Math/RectanglePacker.h"
#include <unordered_map>


namespace se
{
	namespace graphics
	{
		struct GlyphMetrics
		{
			Rectangle rectangle;
			int bearingX = 0;
			int bearingY = 0;
			int advanceX = 0;
		};
		typedef std::unordered_map<CharacterCode, GlyphMetrics> GlyphMap;

		struct FontMetrics
		{
			int height = 0;
			int ascent = 0;
			int descent = 0;
			int maxAdvance = 0;
		};
	}
}
