#pragma once

#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Graphics/CharacterMap.h"
#include "SpehsEngine/Graphics/Types.h"
#include <mutex>
#include <string>


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


			bool loadFace(FontFace& _face, const std::string& _path);
			bool destroyFace(FontFace& _face);

		private:

			void* library = nullptr;
			std::mutex mutex;
		};

		class FontFace // TODO no need for this?
		{
		public:

			FontFace();
			~FontFace();

			FontFace(const FontFace& _other) = delete;
			FontFace& operator=(const FontFace& _other) = delete;

			FontFace(FontFace&& _other) = delete;
			FontFace& operator=(FontFace&& _other) = delete;


			void setSize(const FontSize _size);
			const uint16_t getAtlasSizeEstimate(const size_t numGlyphs) const;
			void* loadGlyph(const CharacterCode _charCode);
			FontMetrics getFontMetrics() const;

		private:

			friend class FontLibrary;

			void* face = nullptr;
			bool loaded = false;
		};
	}
}
