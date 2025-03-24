#include "stdafx.h"
#include "SpehsEngine/Graphics/FontManager.h"

#include "SpehsEngine/Graphics/Internal/FontLibrary.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/OpenSans-Regular.ttf.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/AnonymousPro-Regular.ttf.h"


namespace se
{
	namespace graphics
	{
		FontManager::FontManager()
		{
			fontLibrary = std::make_shared<FontLibrary>();
		}

		std::shared_ptr<Font> FontManager::create(const std::string_view _name, const std::string_view _font, const FontSize _size, const CharacterSet& _charMap)
		{
			return create(_name, _font, _size, TextureModes(), _charMap);
		}

		std::shared_ptr<Font> FontManager::create(const std::string_view _name, const std::string_view _font, const FontSize _size, const TextureModes& _textureModes, const CharacterSet& _charMap)
		{
			const std::string fontPath = pathFinder->getPath(_font);

			for (auto& font : resources)
			{
				if (font->getName() == _name)
				{
					log::warning("Cannot create font '" + _name + "', font with that name already exists!");
					if (font->path != fontPath)
						log::error("Existing font's '" + _name + "' resource path doesn't match!");
					return font;
				}
			}

			resources.push_back(std::make_shared<Font>(_name));
			std::shared_ptr<Font>& font = resources.back();
			font->create(fontPath, _size, _textureModes, _charMap, fontLibrary, resourceLoader);
			return font;
		}
		std::shared_ptr<Font> FontManager::find(const std::string_view _name) const
		{
			for (auto& font : resources)
			{
				if (font->getName() == _name)
					return font;
			}
			return nullptr;
		}

		void FontManager::createDefaultFonts()
		{
			if (defaultFontsCreated)
			{
				log::warning("Default fonts already created!");
				return;
			}
			defaultFontsCreated = true;

			const FontSize defaultFontSize(60, FontSizeType::Pixel);
			const TextureModes defaultTextureModes;

			resources.push_back(std::make_shared<Font>("AnonymousPro-Regular"));
			resources.back()->create(font_anonymousProRegularTtf, sizeof(font_anonymousProRegularTtf), defaultFontSize, defaultTextureModes, defaultCharacterSet, fontLibrary, resourceLoader);

			resources.push_back(std::make_shared<Font>("OpenSans-Regular"));
			resources.back()->create(font_openSansRegularTtf, sizeof(font_openSansRegularTtf), defaultFontSize, defaultTextureModes, defaultCharacterSet, fontLibrary, resourceLoader);
		}
		std::shared_ptr<Font> FontManager::getDefaultFont() const
		{
			se_assert(defaultFontsCreated);
			return find("AnonymousPro-Regular");
		}
	}
}