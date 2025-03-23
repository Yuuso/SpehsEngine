#pragma once

#include "SpehsEngine/Graphics/Resource.h"
#include "SpehsEngine/Graphics/CharacterSet.h"


namespace se
{
	namespace graphics
	{
		class FontLibrary;
		class Texture;

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
			const FontSize& getFontSize() const;
			const FontData* getFontData() const;

			std::shared_ptr<Texture> getDebugTexture();

		private:

			friend class FontManager;
			friend class Text;

			static std::shared_ptr<ResourceData> createResource(const std::string _path, const FontSize _size, CharacterSet _charMap, std::shared_ptr<FontLibrary> _fontLibrary);
			static std::shared_ptr<ResourceData> createResourceFromData(const uint8_t* _data, const size_t _dataSize, const FontSize _size, CharacterSet _charMap, std::shared_ptr<FontLibrary> _fontLibrary);
			void destroy();
			void create(const std::string_view _path, const FontSize _size, const CharacterSet& _charMap, std::shared_ptr<FontLibrary> _fontLibrary, std::shared_ptr<ResourceLoader> _resourceLoader);
			void create(const uint8_t* _data, const size_t _dataSize, const FontSize _size, const CharacterSet& _charMap, std::shared_ptr<FontLibrary> _fontLibrary, std::shared_ptr<ResourceLoader> _resourceLoader);

			std::string path;
			FontSize size;
			CharacterSet charMap;
			std::shared_ptr<FontLibrary> fontLibrary;

			std::shared_ptr<Texture> debugTexture;
		};
	}
}
