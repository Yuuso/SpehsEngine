#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/FontLibrary.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/SE_Assert.h"
#include "SpehsEngine/Graphics/Renderer.h"

#include <ft2build.h>
#include FT_FREETYPE_H


std::string FreetypeErrorMessage(const FT_Error _code)
{
	#if (SE_CONFIGURATION == SE_CONFIGURATION_DEBUG)
		#undef FTERRORS_H_
		#define FT_ERROR_START_LIST		switch (_code) {
		#define FT_ERRORDEF(e, v, s)	case v: return s;
		#define FT_ERROR_END_LIST		}
		#include FT_ERRORS_H
	#endif
	return "Error code: " + std::to_string(_code);
}


namespace se
{
	namespace graphics
	{
		FontLibrary::FontLibrary()
		{
			library = new FT_Library;

			const FT_Error error = FT_Init_FreeType((FT_Library*)library);
			if (error)
			{
				const std::string errorString = FreetypeErrorMessage(error);
				log::error("Freetype FT_Init_FreeType failed! (" + errorString + ")");

				delete (FT_Library*)library;
				library = nullptr;
			}
		}
		FontLibrary::~FontLibrary()
		{
			if (library)
			{
				const FT_Error error = FT_Done_FreeType(*(FT_Library*)library);
				if (error)
				{
					const std::string errorString = FreetypeErrorMessage(error);
					log::error("Freetype FT_Done_FreeType failed! (" + errorString + ")");
				}
				delete (FT_Library*)library;
				library = nullptr;
			}
		}

		bool FontLibrary::loadFace(FontFace& _face, const std::string& _path)
		{
			se_assert(!_face.loaded);
			std::lock_guard lock(mutex);
			const FT_Error error = FT_New_Face(*(FT_Library*)library, _path.c_str(), 0, (FT_Face*)_face.face);
			if (error)
			{
				const std::string errorString = FreetypeErrorMessage(error);
				log::error("Freetype FT_New_Face failed for '" + _path + "'! (" + errorString + ")");
				return false;
			}
			_face.loaded = true;
			return true;
		}
		bool FontLibrary::destroyFace(FontFace& _face)
		{
			se_assert(_face.loaded);
			std::lock_guard lock(mutex);
			const FT_Error error = FT_Done_Face(*(FT_Face*)_face.face);
			if (error)
			{
				const std::string errorString = FreetypeErrorMessage(error);
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
			se_assert(face);
			delete (FT_Face*)face;
		}

		void FontFace::setSize(const FontSize _size)
		{
			if (_size.type == FontSizeType::Point)
			{
				const glm::ivec2 displaySize = Renderer::getDisplaySize();
				FT_Set_Char_Size(*(FT_Face*)face, 0, _size.size * 64, displaySize.x, displaySize.y);
			}
			else if (_size.type == FontSizeType::Pixel)
			{
				FT_Set_Pixel_Sizes(*(FT_Face*)face, 0, _size.size);
			}
			else
			{
				log::error("FontFace::setSize: Unknown FontSizeType!");
			}
		}
		const uint16_t FontFace::getAtlasSizeEstimate(const size_t numGlyphs) const
		{
			return 4096;
			//const uint16_t maxDimension = (uint16_t)(1 + ((*(FT_Face*)face)->size->metrics.height >> 6)) * (uint16_t)ceilf(sqrtf((float)numGlyphs));
			//uint16_t result = 1;
			//while (result < maxDimension)
			//	result <<= 1;
			////assert(result >= 64 && result <= 4096);
			//return result;
		}
		void* FontFace::loadGlyph(const CharacterCode _charCode)
		{
			const FT_UInt glyphIndex = FT_Get_Char_Index(*(FT_Face*)face, _charCode);
			if (glyphIndex == 0)
				return nullptr;

			FT_Error error;
			error = FT_Load_Glyph(*(FT_Face*)face, glyphIndex, FT_LOAD_RENDER);
			if (error)
			{
				const std::string errorString = FreetypeErrorMessage(error);
				log::error("Freetype FT_Load_Glyph failed! (" + errorString + ")");
				return nullptr;
			}
			return (*(FT_Face*)face)->glyph;
		}
		FontMetrics FontFace::getFontMetrics() const
		{
			FontMetrics result;
			result.height = (*(FT_Face*)face)->size->metrics.height >> 6;
			result.ascent = (*(FT_Face*)face)->size->metrics.ascender >> 6;
			result.descent = (*(FT_Face*)face)->size->metrics.descender >> 6;
			result.maxAdvance = (*(FT_Face*)face)->size->metrics.max_advance >> 6;
			return result;
		}
	}
}
