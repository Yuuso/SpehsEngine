#include "stdafx.h"
#include "SpehsEngine/Graphics/Font.h"

#include "bgfx/bgfx.h"
#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Math/RectanglePacker.h"


namespace se
{
	namespace graphics
	{
		Font::Font(const std::string_view _name)
			: Resource(_name)
		{
		}
		Font::~Font()
		{
			destroy();
		}

		void Font::reload(std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			if (!reloadable())
			{
				log::error("Cannot reload font!");
				return;
			}
			destroy();
			create(path, size, charMap, fontLibrary, _resourceLoader);
		}
		bool Font::reloadable() const
		{
			return !path.empty() && fontLibrary;
		}

		void Font::destroy()
		{
			if (!resourceData)
				return;
			resourceData.reset();
		}
		std::shared_ptr<ResourceData> Font::createResource(const std::string _path, const FontSize _size, CharacterMap _charMap, std::shared_ptr<FontLibrary> _fontLibrary)
		{
			se_assert(!_charMap.empty());

			// Always include replacement character
			if (std::find(_charMap.begin(), _charMap.end(), replacementCharacter) == _charMap.end())
				_charMap.push_back(replacementCharacter);

			FontFace fontFace = _fontLibrary->loadFace(_path);
			fontFace.setSize(_size);

			const uint16_t estimatedAtlasSize = fontFace.getAtlasSizeEstimate(_charMap.size());
			RectanglePacker rectanglePacker(estimatedAtlasSize, estimatedAtlasSize);
			bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;

			std::shared_ptr<FontData> result = std::make_shared<FontData>();

			for (auto&& charCode : _charMap)
			{
				auto charCodeToString = [](const CharacterCode charCode) -> std::string
				{
					if (charCode == replacementCharacter)
						return "<replacement character>";
					return std::to_string(charCode);
				};

				if (charCode == U' ')
					continue; // Skip space

				const bool glyphLoaded = fontFace.loadGlyph(charCode);
				if (!glyphLoaded)
				{
					log::warning("Glyph '" + charCodeToString(charCode) + "' not found in font '" + _path + "'.");
					continue;
				}

				if (!bgfx::isValid(textureHandle))
				{
					const bgfx::TextureFormat::Enum textureFormat = (bgfx::TextureFormat::Enum)fontFace.getTextureFormat();
					se_assert(textureFormat != bgfx::TextureFormat::Enum::Unknown);
					textureHandle = bgfx::createTexture2D(
						  uint16_t(estimatedAtlasSize)
						, uint16_t(estimatedAtlasSize)
						, false
						, 1
						, textureFormat
						, TextureModesToFlags(TextureModes())
						, nullptr
						);

					if (bgfx::isValid(textureHandle))
					{
						result->handle = textureHandle.idx;
					}
					else
					{
						log::error("Failed to create font '" + _path + "' texture!");
						continue;
					}
				}

				GlyphMetrics glyphMetrics = fontFace.getGlyphMetrics();
				if (!rectanglePacker.addRectangle(glyphMetrics.rectangle))
				{
					log::error("Failed to add font '" + _path + "' glyph '" + charCodeToString(charCode) + "' to rectangle packer! (size: " + std::to_string(estimatedAtlasSize) + ")");
					continue;
				}

				const bgfx::Memory* memoryBuffer = fontFace.createGlyphMemoryBuffer();
				bgfx::updateTexture2D(textureHandle, 0, 0, glyphMetrics.rectangle.x, glyphMetrics.rectangle.y, glyphMetrics.rectangle.width, glyphMetrics.rectangle.height, memoryBuffer);

				result->glyphMap[charCode] = glyphMetrics;
			}

			result->fontMetrics = fontFace.getFontMetrics();

			_fontLibrary->destroyFace(fontFace);

			return result;
		}
		void Font::create(const std::string_view _path, const FontSize _size, const CharacterMap& _charMap, std::shared_ptr<FontLibrary> _fontLibrary, std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			se_assert(!resourceData);
			se_assert(_fontLibrary);

			path = _path;
			size = _size;
			charMap = _charMap;
			fontLibrary = _fontLibrary;

			if (_resourceLoader)
			{
				std::function<std::shared_ptr<ResourceData>()> func = std::bind(&Font::createResource, path, size, charMap, fontLibrary);
				resourceFuture = _resourceLoader->push(func);
			}
			else
			{
				resourceData = std::dynamic_pointer_cast<FontData>(createResource(path, size, charMap, fontLibrary));
			}
		}

		std::shared_ptr<Texture> Font::getDebugTexture()
		{
			if (debugTexture)
				return debugTexture;

			se_assert(resourceData);
			debugTexture = std::make_shared<Texture>(name + "-debug-tex");
			TextureData data;
			debugTexture->resourceData = std::make_shared<TextureData>();
			debugTexture->resourceData->handle = resourceData->handle;
			return debugTexture;
		}
	}
}