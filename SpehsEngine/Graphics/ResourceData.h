#pragma once

#include "bgfx/bgfx.h" // ?!
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Graphics/ResourceHandle.h"


namespace se
{
	namespace graphics
	{
		struct ResourceData
		{
			virtual ~ResourceData() = default;
			ResourceHandle handle = INVALID_RESOURCE_HANDLE;
		};


		struct ShaderData : ResourceData
		{
			ResourceHandle vertexShaderHandle = INVALID_RESOURCE_HANDLE;
			ResourceHandle fragmentShaderHandle = INVALID_RESOURCE_HANDLE;
		};

		struct TextureData : ResourceData
		{
			bgfx::TextureInfo info;
		};

		struct FontData : ResourceData
		{
			GlyphMap glyphMap;
			Rectangle fillerGlyph;
			FontMetrics fontMetrics;
		};
	}
}
