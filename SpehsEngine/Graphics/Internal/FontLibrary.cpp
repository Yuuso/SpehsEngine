#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/FontLibrary.h"

#include "bgfx/bgfx.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H


static std::string freetypeErrorMessage(const FT_Error _code)
{
	#if (SE_CONFIGURATION == SE_CONFIGURATION_DEBUG)
		#undef FTERRORS_H_
		#define FT_ERROR_START_LIST		switch (_code) {
		#define FT_ERRORDEF(e, v, s)	case v: return s;
		#define FT_ERROR_END_LIST		}
		#include FT_ERRORS_H
	#endif
	return "error code: " + std::to_string(_code);
}


namespace se
{
	namespace graphics
	{
		FontLibrary::FontLibrary()
		{
			library = new FT_Library;

			const FT_Error error = FT_Init_FreeType(library);
			if (error)
			{
				const std::string errorString = freetypeErrorMessage(error);
				log::error("Freetype FT_Init_FreeType failed! (" + errorString + ")");

				delete library;
				library = nullptr;
			}
		}
		FontLibrary::~FontLibrary()
		{
			if (library)
			{
				const FT_Error error = FT_Done_FreeType(*library);
				if (error)
				{
					const std::string errorString = freetypeErrorMessage(error);
					log::error("Freetype FT_Done_FreeType failed! (" + errorString + ")");
				}
				delete library;
				library = nullptr;
			}
		}

		FontFace FontLibrary::loadFace(const std::string& _path)
		{
			std::lock_guard lock(mutex);
			FontFace face;
			const FT_Error error = FT_New_Face(*(FT_Library*)library, _path.c_str(), 0, face.face);
			if (error)
			{
				const std::string errorString = freetypeErrorMessage(error);
				log::error("Freetype FT_New_Face failed for '" + _path + "'! (" + errorString + ")");
				return face;
			}
			face.loaded = true;
			return face;
		}
		bool FontLibrary::destroyFace(FontFace& _face)
		{
			std::lock_guard lock(mutex);
			const FT_Error error = FT_Done_Face(*_face.face);
			if (error)
			{
				const std::string errorString = freetypeErrorMessage(error);
				log::error("Freetype FT_Done_Face failed! (" + errorString + ")");
				return false;
			}
			_face.loaded = false;
			return true;
		}


		FontFace::FontFace()
		{
			face = new FT_Face;
		}
		FontFace::~FontFace()
		{
			se_assert(!loaded);
			if (face)
				delete face;
		}
		FontFace::FontFace(FontFace&& _other)
			: face(std::move(_other.face))
			, loaded(std::move(_other.loaded))
		{
			_other.face = nullptr;
			_other.loaded = false;
		}

		void FontFace::setSize(const FontSize _size)
		{
			if (_size.type == FontSizeType::Point)
			{
				const glm::ivec2 displaySize = Renderer::getDisplaySize();
				FT_Set_Char_Size(*face, 0, _size.size * 64, displaySize.x, displaySize.y);
			}
			else if (_size.type == FontSizeType::Pixel)
			{
				FT_Set_Pixel_Sizes(*face, 0, _size.size);
			}
			else
			{
				log::error("FontFace::setSize: Unknown FontSizeType!");
			}
		}
		const uint16_t FontFace::getAtlasSizeEstimate(const size_t numGlyphs) const
		{
			// TODO: This is for the absolutely unrealistic worst case scenario, need to create better estimation...
			const uint16_t glyphMaxDimension = (uint16_t)((*face)->size->metrics.height >> 6);
			const uint16_t numGlyphRows = (uint16_t)ceilf(sqrtf((float)numGlyphs));
			const uint16_t atlasMaxDimensionEstimate = glyphMaxDimension * numGlyphRows;
			uint16_t bestPowerOfTwo = 1;
			while (bestPowerOfTwo < atlasMaxDimensionEstimate)
				bestPowerOfTwo <<= 1;
			se_assert(bestPowerOfTwo <= 4096); // Shouldn't need such large textures
			return bestPowerOfTwo;
		}
		bool FontFace::loadGlyph(const CharacterCode _charCode)
		{
			const FT_UInt glyphIndex = FT_Get_Char_Index(*face, _charCode);
			if (glyphIndex == 0)
				return false;

			FT_Error error;
			error = FT_Load_Glyph(*face, glyphIndex, FT_LOAD_RENDER);
			if (error)
			{
				const std::string errorString = freetypeErrorMessage(error);
				log::error("Freetype FT_Load_Glyph failed! (" + errorString + ")");
				return false;
			}
			return true;
		}
		int FontFace::getTextureFormat() const
		{
			switch ((*face)->glyph->bitmap.pixel_mode)
			{
				case FT_PIXEL_MODE_NONE:
				case FT_PIXEL_MODE_MAX:
				case FT_PIXEL_MODE_LCD:
				case FT_PIXEL_MODE_LCD_V:
				case FT_PIXEL_MODE_GRAY2:
				case FT_PIXEL_MODE_GRAY4:
				case FT_PIXEL_MODE_MONO:
					log::error("Invalid texture format '" + std::to_string((*face)->glyph->bitmap.pixel_mode) + "'!");
					break;

				case FT_PIXEL_MODE_GRAY:
					return bgfx::TextureFormat::Enum::R8;
					break;
				case FT_PIXEL_MODE_BGRA:
					return bgfx::TextureFormat::Enum::BGRA8;
					break;
			}
			return bgfx::TextureFormat::Enum::Unknown;
		}
		const struct bgfx::Memory* FontFace::createGlyphMemoryBuffer() const
		{
			se_assert((*face)->glyph->bitmap.buffer != nullptr);
			uint32_t bytesPerPixel = 0;
			switch ((*face)->glyph->bitmap.pixel_mode)
			{
				case FT_PIXEL_MODE_NONE:
				case FT_PIXEL_MODE_MAX:
				case FT_PIXEL_MODE_LCD:
				case FT_PIXEL_MODE_LCD_V:
				case FT_PIXEL_MODE_GRAY2:
				case FT_PIXEL_MODE_GRAY4:
				case FT_PIXEL_MODE_MONO:
					break;

				case FT_PIXEL_MODE_GRAY:
					bytesPerPixel = 1;
					break;
				case FT_PIXEL_MODE_BGRA:
					bytesPerPixel = 4;
					break;
			}
			se_assert(bytesPerPixel > 0);
			return bgfx::copy((*face)->glyph->bitmap.buffer, (*face)->glyph->bitmap.width * (*face)->glyph->bitmap.rows * bytesPerPixel);
		}
		GlyphMetrics FontFace::getGlyphMetrics() const
		{
			GlyphMetrics result;
			result.rectangle = Rectangle((uint16_t)(*face)->glyph->bitmap.width, (uint16_t)(*face)->glyph->bitmap.rows);
			result.advanceX = (*face)->glyph->advance.x >> 6;
			result.bearingX = (*face)->glyph->bitmap_left;
			result.bearingY = (*face)->glyph->bitmap_top;
			return result;
		}
		FontMetrics FontFace::getFontMetrics() const
		{
			FontMetrics result;
			result.height = (*face)->size->metrics.height >> 6;
			result.ascent = (*face)->size->metrics.ascender >> 6;
			result.descent = (*face)->size->metrics.descender >> 6;
			result.maxAdvance = (*face)->size->metrics.max_advance >> 6;
			return result;
		}
	}
}
