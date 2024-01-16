#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/FontLoad.h"

#include "SpehsEngine/Graphics/FontMetrics.h"
#include "SpehsEngine/Graphics/Impl/AssetData.h"
#include "SpehsEngine/Graphics/Impl/FontLibrary.h"
#include "SpehsEngine/Math/RectanglePacker.h"


namespace se::gfx::impl
{
	static std::shared_ptr<AssetData> loadFont(FontFace& _fontFace, CharacterSet _charSet, std::string_view _name)
	{
		se_assert(!_charSet.empty());

		// Always include replacement character
		if (std::ranges::find(_charSet, replacementCharacter) == _charSet.end())
		{
			_charSet.insert(replacementCharacter);
		}

		const uint16_t atlasSize = _fontFace.getAtlasSizeEstimate(_charSet.size());
		RectanglePacker rectanglePacker(atlasSize, atlasSize);
		rectanglePacker.setMargin(2); // Margin to prevent bleeding

		bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;

		std::shared_ptr<FontData> result = std::make_shared<FontData>();
		result->width = result->height = atlasSize;

		for (auto&& charCode : _charSet)
		{
			auto charCodeToString =
				[](const CharacterCode charCode) -> std::string
				{
					if (charCode == replacementCharacter)
						return "<replacement character>";
					return std::to_string(charCode);
				};

			if (!_fontFace.loadGlyph(charCode))
			{
				log::warning("Glyph '" + charCodeToString(charCode) + "' not found in font '" + _name + "'.");
				continue;
			}

			// This is here, because a glyph needs to be loaded to get texture format
			if (!bgfx::isValid(textureHandle))
			{
				const bgfx::TextureFormat::Enum textureFormat = _fontFace.getTextureFormat();
				if (textureFormat == bgfx::TextureFormat::Enum::Unknown)
				{
					log::error("Failed to create font '" + _name + "', unknown texture format!");
					return nullptr;
				}

				textureHandle = bgfx::createTexture2D(
					atlasSize
					, atlasSize
					, false
					, 1
					, textureFormat
					, 0
					, nullptr
				);

				if (!bgfx::isValid(textureHandle))
				{
					log::error("Failed to create font '" + _name + "' texture!");
					return nullptr;
				}
				result->init(textureHandle);
			}

			GlyphMetrics glyphMetrics = _fontFace.getGlyphMetrics();

			if (!_fontFace.glyphIsEmpty())
			{

				if (!rectanglePacker.addRectangle(glyphMetrics.rectangle))
				{
					log::error("Failed to add font '" + _name + "' glyph '" + charCodeToString(charCode)
							   + "' to rectangle packer! (size: " + std::to_string(atlasSize) + ")");
					continue;
				}

				const bgfx::Memory* memoryBuffer = _fontFace.createGlyphMemoryBuffer();
				bgfx::updateTexture2D(
					  textureHandle
					, 0
					, 0
					, glyphMetrics.rectangle.x
					, glyphMetrics.rectangle.y
					, glyphMetrics.rectangle.width
					, glyphMetrics.rectangle.height
					, memoryBuffer);
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
			uint8_t fillerBuffer[maxFillerBufferSize]{};
			memset(&fillerBuffer[0], 255, (size_t)maxFillerBufferSize);

			const uint32_t bytesPerPixel = _fontFace.getBytesPerPixel();
			const uint32_t actualBufferSize = fillerGlyphSize * fillerGlyphSize * bytesPerPixel;
			se_assert(actualBufferSize <= maxFillerBufferSize);
			const bgfx::Memory* memoryBuffer = bgfx::copy(&fillerBuffer[0], actualBufferSize);
			bgfx::updateTexture2D(
				  textureHandle
				, 0
				, 0
				, result->fillerGlyph.x
				, result->fillerGlyph.y
				, result->fillerGlyph.width
				, result->fillerGlyph.height
				, memoryBuffer);

			// Offset to prevent bleeding
			result->fillerGlyph.x += fillerGlyphOffset;
			result->fillerGlyph.y += fillerGlyphOffset;
			result->fillerGlyph.height -= fillerGlyphOffset * 2;
			result->fillerGlyph.width -= fillerGlyphOffset * 2;
		}
		else
		{
			log::error("Failed to add filler glyph for font '" + _name +
					   "' to rectangle packer! (size: " + std::to_string(atlasSize) + ")");
			result->fillerGlyph = Rectangle();
		}

		result->fontMetrics = _fontFace.getFontMetrics();
		result->fontMetrics.textureSize = atlasSize;
		return result;
	}
	std::shared_ptr<AssetData> loadFont(std::string _path, FontSize _size, CharacterSet _charSet)
	{
		FontLibrary library;
		if (!library.isValid())
			return nullptr;

		FontFace fontFace = library.loadFace(_path);
		if (!fontFace.isValid())
			return nullptr;

		fontFace.setSize(_size);
		return loadFont(fontFace, _charSet, _path);
	}
	std::shared_ptr<AssetData> loadFont(const uint8_t* _data, size_t _dataSize, FontSize _size, CharacterSet _charSet)
	{
		FontLibrary library;
		if (!library.isValid())
			return nullptr;

		FontFace fontFace = library.loadFace(_data, _dataSize);
		if (!fontFace.isValid())
			return nullptr;

		fontFace.setSize(_size);
		return loadFont(fontFace, _charSet, "font_from_data");
	}
}
