#include "stdafx.h"
#include "SpehsEngine/Graphics/Font.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Graphics/FontMetrics.h"
#include "SpehsEngine/Graphics/Internal/FontLibrary.h"
#include "SpehsEngine/Graphics/Texture.h"
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
		const FontSize& Font::getFontSize() const
		{
			return size;
		}

		void Font::destroy()
		{
			if (!resourceData)
				return;
			resourceData.reset();
		}

		static std::shared_ptr<ResourceData> createResource(FontFace& _fontFace, CharacterSet _charMap, const std::string_view _name)
		{
			se_assert(!_charMap.empty());

			// Always include replacement character
			if (std::find(_charMap.begin(), _charMap.end(), replacementCharacter) == _charMap.end())
				_charMap.insert(replacementCharacter);

			const uint16_t atlasSize = _fontFace.getAtlasSizeEstimate(_charMap.size());
			RectanglePacker rectanglePacker(atlasSize, atlasSize);
			rectanglePacker.setMargin(2); // Margin to prevent bleeding
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

				const bool glyphLoaded = _fontFace.loadGlyph(charCode);
				if (!glyphLoaded)
				{
					log::warning("Glyph '" + charCodeToString(charCode) + "' not found in font '" + _name + "'.");
					continue;
				}

				GlyphMetrics glyphMetrics = _fontFace.getGlyphMetrics();

				if (!_fontFace.glyphIsEmpty())
				{
					if (!bgfx::isValid(textureHandle))
					{
						const bgfx::TextureFormat::Enum textureFormat = _fontFace.getTextureFormat();
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
							log::error("Failed to create font '" + _name + "' texture!");
							continue;
						}
					}

					if (!rectanglePacker.addRectangle(glyphMetrics.rectangle))
					{
						log::error("Failed to add font '" + _name + "' glyph '" + charCodeToString(charCode) + "' to rectangle packer! (size: " + std::to_string(atlasSize) + ")");
						continue;
					}

					const bgfx::Memory* memoryBuffer = _fontFace.createGlyphMemoryBuffer();
					bgfx::updateTexture2D(textureHandle, 0, 0, glyphMetrics.rectangle.x, glyphMetrics.rectangle.y, glyphMetrics.rectangle.width, glyphMetrics.rectangle.height, memoryBuffer);
				}

				result->glyphMap[charCode] = glyphMetrics;
			}

			constexpr uint16_t fillerGlyphOffset = 2;
			constexpr uint32_t fillerGlyphSize = 2 + fillerGlyphOffset * 2;
			result->fillerGlyph.height = (uint16_t)fillerGlyphSize;
			result->fillerGlyph.width = (uint16_t)fillerGlyphSize;
			if (rectanglePacker.addRectangle(result->fillerGlyph))
			{
				constexpr uint32_t maxBytesPerPixel = 4;
				constexpr uint32_t maxFillerBufferSize = fillerGlyphSize * fillerGlyphSize * maxBytesPerPixel;
				uint8_t fillerBuffer[maxFillerBufferSize];
				memset(&fillerBuffer[0], 255, (size_t)maxFillerBufferSize);

				const uint32_t bytesPerPixel = _fontFace.getBytesPerPixel();
				const uint32_t actualBufferSize = fillerGlyphSize * fillerGlyphSize * bytesPerPixel;
				se_assert(actualBufferSize <= maxFillerBufferSize);
				const bgfx::Memory* memoryBuffer = bgfx::copy(&fillerBuffer[0], actualBufferSize);
				bgfx::updateTexture2D(textureHandle, 0, 0, result->fillerGlyph.x, result->fillerGlyph.y, result->fillerGlyph.width, result->fillerGlyph.height, memoryBuffer);

				// Offset to prevent bleeding
				result->fillerGlyph.x += fillerGlyphOffset;
				result->fillerGlyph.y += fillerGlyphOffset;
				result->fillerGlyph.height -= fillerGlyphOffset * 2;
				result->fillerGlyph.width -= fillerGlyphOffset * 2;
			}
			else
			{
				log::error("Failed to add filler glyph for font '" + _name + "' to rectangle packer! (size: " + std::to_string(atlasSize) + ")");
				result->fillerGlyph = Rectangle();
			}

			result->fontMetrics = _fontFace.getFontMetrics();
			result->fontMetrics.textureSize = atlasSize;
			return result;
		}
		std::shared_ptr<ResourceData> Font::createResource(const std::string _path, const FontSize _size, CharacterSet _charMap, std::shared_ptr<FontLibrary> _fontLibrary)
		{
			FontFace fontFace = _fontLibrary->loadFace(_path);
			fontFace.setSize(_size);
			std::shared_ptr<ResourceData> result = graphics::createResource(fontFace, _charMap, _path);
			_fontLibrary->destroyFace(fontFace);
			return result;
		}
		std::shared_ptr<ResourceData> Font::createResourceFromData(const uint8_t* _data, const size_t _dataSize, const FontSize _size, CharacterSet _charMap, std::shared_ptr<FontLibrary> _fontLibrary)
		{
			FontFace fontFace = _fontLibrary->loadFace(_data, _dataSize);
			fontFace.setSize(_size);
			std::shared_ptr<ResourceData> result = graphics::createResource(fontFace, _charMap, "embedded");
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
		void Font::create(const uint8_t* _data, const size_t _dataSize, const FontSize _size, const CharacterSet& _charMap, std::shared_ptr<FontLibrary> _fontLibrary, std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			se_assert(!resourceData);
			se_assert(_fontLibrary);

			path = "";
			size = _size;
			charMap = _charMap;
			fontLibrary = _fontLibrary;

			if (_resourceLoader)
			{
				std::function<std::shared_ptr<ResourceData>()> func = std::bind(&Font::createResourceFromData, _data, _dataSize, size, charMap, fontLibrary);
				resourceFuture = _resourceLoader->push(func);
			}
			else
			{
				resourceData = std::dynamic_pointer_cast<FontData>(createResourceFromData(_data, _dataSize, size, charMap, fontLibrary));
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