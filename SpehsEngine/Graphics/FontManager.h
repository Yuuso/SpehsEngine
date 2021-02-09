#pragma once

#include "SpehsEngine/Graphics/Internal/FontLibrary.h"
#include "SpehsEngine/Graphics/Internal/ResourceManager.h"
#include "SpehsEngine/Graphics/CharacterMap.h"
#include "SpehsEngine/Graphics/Font.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"
#include "SpehsEngine/Graphics/ResourcePathFinder.h"
#include <memory>
#include <vector>


namespace se
{
	namespace graphics
	{
		class FontManager : public ResourceManager<Font>
		{
		public:

			FontManager();
			~FontManager() = default;

			FontManager(const FontManager& _other) = delete;
			FontManager& operator=(const FontManager& _other) = delete;

			FontManager(FontManager&& _other) = delete;
			FontManager& operator=(FontManager&& _other) = delete;


			std::shared_ptr<Font> create(const std::string_view _name, const std::string_view _font, const FontSize _size, const CharacterMap& _charMap = defaultCharacterMap);
			std::shared_ptr<Font> find(const std::string_view _name) const;

		private:

			std::shared_ptr<FontLibrary> fontLibrary;
		};
	}
}
