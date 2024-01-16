#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/FontLibrary.h"

#include "SpehsEngine/Graphics/Display.h"

#include <ft2build.h>
#include FT_FREETYPE_H


static std::string freetypeErrorMessage(FT_Error _code)
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

FontLibrary::FontLibrary()
{
	const FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		const std::string errorString = freetypeErrorMessage(error);
		log::error("Freetype FT_Init_FreeType failed! (" + errorString + ")");
	}
}
FontLibrary::~FontLibrary()
{
	if (library)
	{
		const FT_Error error = FT_Done_FreeType(library);
		if (error)
		{
			const std::string errorString = freetypeErrorMessage(error);
			log::error("Freetype FT_Done_FreeType failed! (" + errorString + ")");
		}
	}
}
bool FontLibrary::isValid() const
{
	return library != nullptr;
}
FontFace FontLibrary::loadFace(const std::string& _path)
{
	FT_Face face;
	const FT_Error error = FT_New_Face(library, _path.c_str(), 0, &face);
	if (error)
	{
		const std::string errorString = freetypeErrorMessage(error);
		log::error("Freetype FT_New_Face failed for '" + _path + "'! (" + errorString + ")");
		return FontFace();
	}
	return FontFace(face);
}
FontFace FontLibrary::loadFace(const uint8_t* _data, size_t _dataSize)
{
	FT_Face face;
	const FT_Error error = FT_New_Memory_Face(library, (const FT_Byte*)_data, (FT_Long)_dataSize, 0, &face);
	if (error)
	{
		const std::string errorString = freetypeErrorMessage(error);
		log::error("Freetype FT_New_Memory_Face failed! (" + errorString + ")");
		return FontFace();
	}
	return FontFace(face);
}

FontFace::FontFace(FT_Face _face)
	: face(_face)
{}
FontFace::FontFace(FontFace&& _other)
	: face(_other.face)
{
	_other.face = nullptr;
}
FontFace::~FontFace()
{
	if (face)
	{
		const FT_Error error = FT_Done_Face(face);
		if (error)
		{
			const std::string errorString = freetypeErrorMessage(error);
			log::error("Freetype FT_Done_Face failed! (" + errorString + ")");
		}
	}
}
bool FontFace::isValid() const
{
	return face != nullptr;
}
bool FontFace::glyphIsEmpty() const
{
	se_assert(isValid());
	return face->glyph->bitmap.buffer == nullptr;
}
uint16_t FontFace::getAtlasSizeEstimate(size_t numGlyphs) const
{
	se_assert(isValid());
	// TODO: This is for the absolutely unrealistic worst case scenario, need to create better estimation...
	const uint16_t glyphMaxDimension = (uint16_t)(face->size->metrics.height >> 6);
	const uint16_t numGlyphRows = (uint16_t)ceilf(sqrtf((float)numGlyphs));
	const uint16_t atlasMaxDimensionEstimate = glyphMaxDimension * numGlyphRows;
	uint16_t bestPowerOfTwo = 1;
	while (bestPowerOfTwo < atlasMaxDimensionEstimate)
	{
		bestPowerOfTwo <<= 1;
	}
	se_assert(bestPowerOfTwo <= 4096); // Shouldn't need such large textures
	return bestPowerOfTwo;
}
bgfx::TextureFormat::Enum FontFace::getTextureFormat() const
{
	se_assert(isValid());
	switch (face->glyph->bitmap.pixel_mode)
	{
		case FT_PIXEL_MODE_NONE:
		case FT_PIXEL_MODE_MAX:
		case FT_PIXEL_MODE_LCD:
		case FT_PIXEL_MODE_LCD_V:
		case FT_PIXEL_MODE_GRAY2:
		case FT_PIXEL_MODE_GRAY4:
		case FT_PIXEL_MODE_MONO:
			log::error("Unsupported font pixel mode '" + std::to_string(face->glyph->bitmap.pixel_mode) + "'!");
			break;

		case FT_PIXEL_MODE_GRAY:
			return bgfx::TextureFormat::Enum::R8;
		case FT_PIXEL_MODE_BGRA:
			return bgfx::TextureFormat::Enum::BGRA8;
	}
	return bgfx::TextureFormat::Enum::Unknown;
}
uint32_t FontFace::getBytesPerPixel() const
{
	se_assert(isValid());
	switch (face->glyph->bitmap.pixel_mode)
	{
		case FT_PIXEL_MODE_NONE:
		case FT_PIXEL_MODE_MAX:
		case FT_PIXEL_MODE_LCD:
		case FT_PIXEL_MODE_LCD_V:
		case FT_PIXEL_MODE_GRAY2:
		case FT_PIXEL_MODE_GRAY4:
		case FT_PIXEL_MODE_MONO:
			log::error("Unsupported font pixel mode '" + std::to_string(face->glyph->bitmap.pixel_mode) + "'!");
			break;

		case FT_PIXEL_MODE_GRAY:
			return 1;
		case FT_PIXEL_MODE_BGRA:
			return 4;
	}
	return 0;
}
GlyphMetrics FontFace::getGlyphMetrics() const
{
	se_assert(isValid());
	GlyphMetrics result;
	result.rectangle = glyphIsEmpty()
		? Rectangle()
		: Rectangle((uint16_t)face->glyph->bitmap.width, (uint16_t)face->glyph->bitmap.rows);
	result.advanceX = face->glyph->advance.x >> 6;
	result.bearingX = face->glyph->bitmap_left;
	result.bearingY = face->glyph->bitmap_top;
	return result;
}
FontMetrics FontFace::getFontMetrics() const
{
	se_assert(isValid());
	FontMetrics result;
	result.height = face->size->metrics.height >> 6;
	result.ascent = face->size->metrics.ascender >> 6;
	result.descent = face->size->metrics.descender >> 6;
	result.maxAdvance = face->size->metrics.max_advance >> 6;
	return result;
}
const bgfx::Memory* FontFace::createGlyphMemoryBuffer() const
{
	se_assert(isValid());
	se_assert(!glyphIsEmpty());
	const uint32_t bytesPerPixel = getBytesPerPixel();
	se_assert(bytesPerPixel > 0);
	return bgfx::copy(
		face->glyph->bitmap.buffer,
		face->glyph->bitmap.width * face->glyph->bitmap.rows * bytesPerPixel);
}
void FontFace::setSize(FontSize _size)
{
	se_assert(isValid());
	if (_size.type == FontSizeType::Point)
	{
		const DisplayDPI dpi = getDisplayList().front().getDPI();
		FT_Set_Char_Size(face, 0, _size.size << 6, (FT_UInt)dpi.horizontalDPI, (FT_UInt)dpi.verticalDPI);
	}
	else if (_size.type == FontSizeType::Pixel)
	{
		FT_Set_Pixel_Sizes(face, 0, _size.size);
	}
	else
	{
		log::error("FontFace::setSize: Unknown FontSizeType!");
	}
}
bool FontFace::loadGlyph(CharacterCode _charCode)
{
	se_assert(isValid());
	const FT_UInt glyphIndex = FT_Get_Char_Index(face, _charCode);
	if (glyphIndex == 0)
		return false;

	const FT_Error error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_RENDER);
	if (error)
	{
		const std::string errorString = freetypeErrorMessage(error);
		log::error("Freetype FT_Load_Glyph failed! (" + errorString + ")");
		return false;
	}
	return true;
}
