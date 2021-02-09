#pragma once

#include "SpehsEngine/Graphics/Internal/FontLibrary.h"
#include "SpehsEngine/Graphics/Internal/Resource.h"
#include "SpehsEngine/Graphics/CharacterMap.h"
#include "SpehsEngine/Graphics/Texture.h"
#include <memory>


namespace se
{
	namespace graphics
	{
		class Font : public Resource<FontData>
		{
		public:

			Font() = delete;
			Font(const std::string_view _name);
			~Font();

			Font(const Font& _other) = delete;
			Font& operator=(const Font& _other) = delete;

			Font(Font&& _other) = delete;
			Font& operator=(Font&& _other) = delete;


			void reload(std::shared_ptr<ResourceLoader> _resourceLoader = nullptr) override;
			bool reloadable() const override;

			std::shared_ptr<Texture> getDebugTexture();

		private:

			friend class FontManager;

			static std::shared_ptr<ResourceData> createResource(const std::string _path, const FontSize _size, CharacterMap _charMap, std::shared_ptr<FontLibrary> _fontLibrary);
			void destroy();
			void create(const std::string_view _path, const FontSize _size, const CharacterMap& _charMap, std::shared_ptr<FontLibrary> _fontLibrary, std::shared_ptr<ResourceLoader> _resourceLoader);

			std::string path;
			FontSize size;
			CharacterMap charMap;
			std::shared_ptr<FontLibrary> fontLibrary;

			std::shared_ptr<Texture> debugTexture;
		};
	}
}
