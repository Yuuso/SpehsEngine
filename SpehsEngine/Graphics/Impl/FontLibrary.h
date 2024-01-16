#pragma once

#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/FontMetrics.h"


typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;

namespace se::gfx::impl
{
	class FontFace
	{
	public:

		SE_NO_COPY(FontFace);
		FontFace& operator=(FontFace&& _other) = delete;
		FontFace() = default;
		FontFace(FT_Face _face);
		FontFace(FontFace&& _other);
		~FontFace();

		bool						isValid() const;
		bool						glyphIsEmpty() const;
		uint16_t					getAtlasSizeEstimate(size_t numGlyphs) const;
		bgfx::TextureFormat::Enum	getTextureFormat() const;
		uint32_t					getBytesPerPixel() const;
		GlyphMetrics				getGlyphMetrics() const;
		FontMetrics					getFontMetrics() const;
		const bgfx::Memory* createGlyphMemoryBuffer() const;
		void						setSize(FontSize _size);
		bool						loadGlyph(CharacterCode _charCode);

	private:

		FT_Face face = nullptr;
	};

	class FontLibrary
	{
	public:

		SE_NO_COPY_OR_MOVE(FontLibrary);
		FontLibrary();
		~FontLibrary();

		bool isValid() const;
		FontFace loadFace(const std::string& _path);
		FontFace loadFace(const uint8_t* _data, const size_t _dataSize);

	private:

		FT_Library library = nullptr;
	};
}
