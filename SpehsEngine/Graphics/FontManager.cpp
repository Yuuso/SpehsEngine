#include "stdafx.h"
#include "SpehsEngine/Graphics/FontManager.h"

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/OpenSans-Regular.ttf.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/AnonymousPro-Regular.ttf.h"


namespace se
{
	namespace graphics
	{
		FontManager::FontManager()
		{
			fontLibrary = std::make_shared<FontLibrary>();

			// Embedded fonts
			{
				const FontSize defaultFontSize(18, FontSizeType::Pixel);

				resources.push_back(std::make_shared<Font>("AnonymousPro-Regular"));
				resources.back()->create(font_anonymousProRegularTtf, sizeof(font_anonymousProRegularTtf), defaultFontSize, defaultCharacterSet, fontLibrary, resourceLoader);

				resources.push_back(std::make_shared<Font>("OpenSans-Regular"));
				resources.back()->create(font_openSansRegularTtf, sizeof(font_openSansRegularTtf), defaultFontSize, defaultCharacterSet, fontLibrary, resourceLoader);
			}
		}

		std::shared_ptr<Font> FontManager::create(const std::string_view _name, const std::string_view _font, const FontSize _size, const CharacterSet& _charMap)
		{
			const std::string fontPath = pathFinder->getPath(_font);

			for (auto& font : resources)
			{
				if (font->getName() == _name)
				{
					log::warning("Cannot create font '" + _name + "', font with that name already exists!");
					se_assert(font->path == fontPath);
					return font;
				}
			}

			resources.push_back(std::make_shared<Font>(_name));
			std::shared_ptr<Font>& font = resources.back();
			font->create(fontPath, _size, _charMap, fontLibrary, resourceLoader);
			return font;
		}
		std::shared_ptr<Font> FontManager::find(const std::string_view _name) const
		{
			for (auto& font : resources)
			{
				if (font->getName() == _name)
					return font;
			}
			se_assert_m(false, "Font '" + std::string(_name) + "' not found!");
			return nullptr;
		}
		std::shared_ptr<Font> FontManager::getDefaultFont() const
		{
			se_assert(resources.size() > 0);
			return resources[0];
		}
	}
}