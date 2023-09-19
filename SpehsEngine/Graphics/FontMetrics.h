#pragma once

#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Math/RectanglePacker.h"


namespace se::gfx
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
		uint16_t textureSize = 0;
		int height = 0;
		int ascent = 0;
		int descent = 0;
		int maxAdvance = 0;
	};
}
