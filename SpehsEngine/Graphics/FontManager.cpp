#include "stdafx.h"
#include "SpehsEngine/Graphics/FontManager.h"

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"


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

			std::shared_ptr<Font>& font = resources.emplace_back(std::make_shared<Font>(_name));
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
	}
}