#pragma once

#include "SpehsEngine/Graphics/ResourceManager.h"
#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/Font.h"


/*
	LIST OF DEFAULT FONTS

	- AnonymousPro-Regular
	- OpenSans-Regular
*/


namespace se
{
	namespace graphics
	{
		class FontLibrary;

		class FontManager : public ResourceManager<Font>
		{
		public:

			FontManager();
			~FontManager() = default;

			FontManager(const FontManager& _other) = delete;
			FontManager& operator=(const FontManager& _other) = delete;

			FontManager(FontManager&& _other) = delete;
			FontManager& operator=(FontManager&& _other) = delete;


			std::shared_ptr<Font> create(const std::string_view _name, const std::string_view _font, const FontSize _size, const CharacterSet& _charMap = defaultCharacterSet);
			std::shared_ptr<Font> create(const std::string_view _name, const std::string_view _font, const FontSize _size, const TextureModes& _textureModes, const CharacterSet& _charMap = defaultCharacterSet);
			std::shared_ptr<Font> find(const std::string_view _name) const;

			void createDefaultFonts();
			std::shared_ptr<Font> getDefaultFont() const;

		private:

			std::shared_ptr<FontLibrary> fontLibrary;
			bool defaultFontsCreated = false;
		};
	}
}
