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
		std::shared_ptr<ResourceData> Font::createResource(const std::string _path, const FontSize _size, CharacterSet _charMap, std::shared_ptr<FontLibrary> _fontLibrary)
		{
			se_assert(!_charMap.empty());

			// Always include replacement character
			if (std::find(_charMap.begin(), _charMap.end(), replacementCharacter) == _charMap.end())
				_charMap.insert(replacementCharacter);

			FontFace fontFace = _fontLibrary->loadFace(_path);
			fontFace.setSize(_size);

			const uint16_t atlasSize = fontFace.getAtlasSizeEstimate(_charMap.size());
			RectanglePacker rectanglePacker(atlasSize, atlasSize);
			rectanglePacker.setMargin(2);
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

				const bool glyphLoaded = fontFace.loadGlyph(charCode);
				if (!glyphLoaded)
				{
					log::warning("Glyph '" + charCodeToString(charCode) + "' not found in font '" + _path + "'.");
					continue;
				}

				GlyphMetrics glyphMetrics = fontFace.getGlyphMetrics();

				if (!fontFace.glyphIsEmpty())
				{
					if (!bgfx::isValid(textureHandle))
					{
						const bgfx::TextureFormat::Enum textureFormat = (bgfx::TextureFormat::Enum)fontFace.getTextureFormat();
						se_assert(textureFormat != bgfx::TextureFormat::Enum::Unknown);
						textureHandle = bgfx::createTexture2D(
							uint16_t(atlasSize)
							, uint16_t(atlasSize)
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

					if (!rectanglePacker.addRectangle(glyphMetrics.rectangle))
					{
						log::error("Failed to add font '" + _path + "' glyph '" + charCodeToString(charCode) + "' to rectangle packer! (size: " + std::to_string(atlasSize) + ")");
						continue;
					}

					const bgfx::Memory* memoryBuffer = fontFace.createGlyphMemoryBuffer();
					bgfx::updateTexture2D(textureHandle, 0, 0, glyphMetrics.rectangle.x, glyphMetrics.rectangle.y, glyphMetrics.rectangle.width, glyphMetrics.rectangle.height, memoryBuffer);
				}

				result->glyphMap[charCode] = glyphMetrics;
			}

			constexpr uint32_t fillerGlyphSize = 3;
			result->fillerGlyph.height = (uint16_t)fillerGlyphSize;
			result->fillerGlyph.width = (uint16_t)fillerGlyphSize;
			if (rectanglePacker.addRectangle(result->fillerGlyph))
			{
				constexpr uint32_t maxBytesPerPixel = 4;
				constexpr uint32_t maxFillerBufferSize = fillerGlyphSize * fillerGlyphSize * maxBytesPerPixel;
				uint8_t fillerBuffer[maxFillerBufferSize];
				memset(&fillerBuffer[0], 255, (size_t)maxFillerBufferSize);

				const uint32_t bytesPerPixel = fontFace.getBytesPerPixel();
				const uint32_t actualBufferSize = fillerGlyphSize * fillerGlyphSize * bytesPerPixel;
				se_assert(actualBufferSize <= maxFillerBufferSize);
				const bgfx::Memory* memoryBuffer = bgfx::copy(&fillerBuffer[0], actualBufferSize);
				bgfx::updateTexture2D(textureHandle, 0, 0, result->fillerGlyph.x, result->fillerGlyph.y, result->fillerGlyph.width, result->fillerGlyph.height, memoryBuffer);
			}
			else
			{
				log::error("Failed to add filler glyph for font '" + _path + "' to rectangle packer! (size: " + std::to_string(atlasSize) + ")");
				result->fillerGlyph = Rectangle();
			}

			result->fontMetrics = fontFace.getFontMetrics();
			result->fontMetrics.textureSize = atlasSize;
			_fontLibrary->destroyFace(fontFace);
			return result;
		}
		void Font::create(const std::string_view _path, const FontSize _size, const CharacterSet& _charMap, std::shared_ptr<FontLibrary> _fontLibrary, std::shared_ptr<ResourceLoader> _resourceLoader)
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