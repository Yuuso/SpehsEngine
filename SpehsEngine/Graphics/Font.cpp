#include "stdafx.h"
#include "SpehsEngine/Graphics/Font.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "SpehsEngine/Graphics/Internal/InternalTypes.h"
#include "SpehsEngine/Graphics/Internal/FontMetrics.h"
#include "SpehsEngine/Math/RectanglePacker.h"

#include <ft2build.h>
#include FT_FREETYPE_H


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

			FontFace fontFace;
			_fontLibrary->loadFace(fontFace, _path);
			fontFace.setSize(_size);

			const uint16_t estimatedAtlasSize = fontFace.getAtlasSizeEstimate(_charMap.size());
			RectanglePacker rectanglePacker(estimatedAtlasSize, estimatedAtlasSize);
			bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;

			std::shared_ptr<FontData> result = std::make_shared<FontData>();

			uint32_t bytesPerPixel = 0;
			bgfx::TextureFormat::Enum textureFormat = bgfx::TextureFormat::Enum::Unknown;

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

				const FT_GlyphSlot glyphSlot = (FT_GlyphSlot)fontFace.loadGlyph(charCode);
				if (glyphSlot == nullptr)
				{
					log::warning("Glyph '" + charCodeToString(charCode) + "' not found in font '" + _path + "'.");
					continue;
				}
				se_assert(glyphSlot->bitmap.buffer != nullptr);

				if (!bgfx::isValid(textureHandle))
				{
					switch (glyphSlot->bitmap.pixel_mode)
					{
						case FT_PIXEL_MODE_NONE:
						case FT_PIXEL_MODE_MAX:
						case FT_PIXEL_MODE_LCD:
						case FT_PIXEL_MODE_LCD_V:
						case FT_PIXEL_MODE_GRAY2:
						case FT_PIXEL_MODE_GRAY4:
						case FT_PIXEL_MODE_MONO:
							log::error("Invalid texture format '" + std::to_string(glyphSlot->bitmap.pixel_mode) + "'!");
							break;

						case FT_PIXEL_MODE_GRAY:
							textureFormat = bgfx::TextureFormat::Enum::R8;
							bytesPerPixel = 1;
							break;
						case FT_PIXEL_MODE_BGRA:
							textureFormat = bgfx::TextureFormat::Enum::BGRA8;
							bytesPerPixel = 4;
							break;
					}

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

				Rectangle glyphRect((uint16_t)glyphSlot->bitmap.width, (uint16_t)glyphSlot->bitmap.rows);
				if (!rectanglePacker.addRectangle(glyphRect))
				{
					log::error("Failed to add font '" + _path + "' glyph '" + charCodeToString(charCode) + "' to rectangle packer! (size: " + std::to_string(estimatedAtlasSize) + ")");
					continue;
				}

				se_assert(bytesPerPixel > 0);
				const bgfx::Memory* memoryBuffer = bgfx::copy(glyphSlot->bitmap.buffer, glyphSlot->bitmap.width * glyphSlot->bitmap.rows * bytesPerPixel);
				bgfx::updateTexture2D(textureHandle, 0, 0, glyphRect.x, glyphRect.y, glyphRect.width, glyphRect.height, memoryBuffer);

				GlyphMetrics& glyphMetrics = result->glyphMap[charCode];
				glyphMetrics.rectangle = glyphRect;
				glyphMetrics.advanceX = glyphSlot->advance.x >> 6;
				glyphMetrics.bearingX = glyphSlot->bitmap_left;
				glyphMetrics.bearingY = glyphSlot->bitmap_top;
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