#pragma once

#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/Types.h"
#include <mutex>
#include <string>


typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace se
{
	namespace graphics
	{
		class FontFace;

		class FontLibrary
		{
		public:

			FontLibrary();
			~FontLibrary();

			FontLibrary(const FontLibrary& _other) = delete;
			FontLibrary& operator=(const FontLibrary& _other) = delete;

			FontLibrary(FontLibrary&& _other) = delete;
			FontLibrary& operator=(FontLibrary&& _other) = delete;


			FontFace loadFace(const std::string& _path);
			FontFace loadFace(const uint8_t* _data, const size_t _dataSize);
			bool destroyFace(FontFace& _face);

		private:

			FT_Library* library = nullptr;
			std::mutex mutex;
		};


		class FontFace
		{
		public:

			FontFace();
			~FontFace();

			FontFace(const FontFace& _other) = delete;
			FontFace& operator=(const FontFace& _other) = delete;

			FontFace(FontFace&& _other);
			FontFace& operator=(FontFace&& _other) = delete;


			void setSize(const FontSize _size);
			const uint16_t getAtlasSizeEstimate(const size_t numGlyphs) const;
			bool loadGlyph(const CharacterCode _charCode);
			bool glyphIsEmpty() const;
			int getTextureFormat() const; // bgfx::TextureFormat::Enum
			uint32_t getBytesPerPixel() const;
			const struct bgfx::Memory* createGlyphMemoryBuffer() const;
			GlyphMetrics getGlyphMetrics() const;
			FontMetrics getFontMetrics() const;

		private:

			friend class FontLibrary;

			FT_Face* face = nullptr;
			bool loaded = false;
		};
	}
}
