#include "stdafx.h"
#include "SpehsEngine/Graphics/Impl/TextureLoad.h"

#include "SpehsEngine/Graphics/Impl/AssetData.h"
#include "bx/allocator.h"
#include "bx/error.h"
#include "bimg/bimg.h"
#include "bimg/decode.h"
#include <fstream>
#include <iterator>


namespace se::gfx::impl
{
	bool getImageDimensions(const std::string& _path, uint16_t& _width, uint16_t& _height)
	{
		if (!_path.ends_with(".png"))
			return false;

		std::ifstream input(_path, std::ios::binary | std::ios::ate);
		if (!input.is_open())
		{
			log::error("Couldn't open png file!");
			return false;
		}

		static constexpr uint8_t header1[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
		char header1Data[sizeof(header1)] = {};
		input.seekg(0);
		input.read(header1Data, sizeof(header1Data));
		if (memcmp(header1, header1Data, sizeof(header1)) != 0)
		{
			log::error("Invalid png header 1!");
			return false;
		}

		static constexpr uint8_t header2[] = { 0x49, 0x48, 0x44, 0x52 };
		char header2Data[sizeof(header2)] = {};
		input.seekg(12);
		input.read(header2Data, sizeof(header2Data));
		if (memcmp(header2, header2Data, sizeof(header2)) != 0)
		{
			log::error("Invalid png header 2!");
			return false;
		}

		unsigned int width = 0;
		unsigned int height = 0;
		input.seekg(16);
		input.read((char*)&width, 4);
		input.read((char*)&height, 4);

		if (!input.good())
		{
			log::error("Error reading png file!");
			return false;
		}

		_width = (uint16_t)networkToHost(width);
		_height = (uint16_t)networkToHost(height);
		return true;
	}
	std::shared_ptr<AssetData> loadTexture(std::string _path, TextureModes _textureModes)
	{
		std::ifstream input(_path, std::ios::binary | std::ios::ate);
		if (!input.is_open())
		{
			log::error("Failed to open texture: " + _path);
			return nullptr;
		}

		std::vector<char> data(static_cast<size_t>(input.tellg()));
		input.seekg(0, std::ios::beg);
		input.read(data.data(), data.size());
		if (data.empty())
		{
			log::error("Failed to read texture data: " + _path);
			return nullptr;
		}

		static bx::DefaultAllocator defaultAllocator;
		bx::Error error;
		bimg::ImageContainer* imageContainer = bimg::imageParse(
			&defaultAllocator,
			data.data(),
			static_cast<uint32_t>(data.size()),
			bimg::TextureFormat::Count,
			&error);
		input.close();
		if (imageContainer == nullptr)
		{
			log::error("Failed to parse texture: " + _path + ", error: " +
				std::string(error.getMessage().getPtr(), (size_t)error.getMessage().getLength()));
			return nullptr;
		}

		auto imageReleaseCb = []([[maybe_unused]] void* _ptr, void* _userData)
			{
				bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
				bimg::imageFree(imageContainer);
			};

		const bgfx::Memory* mem = bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, imageReleaseCb, imageContainer);
		const uint64_t flags = TextureModesToFlags(_textureModes);

		bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;
		if (imageContainer->m_cubeMap)
		{
			textureHandle = bgfx::createTextureCube(
				uint16_t(imageContainer->m_width)
				, 1 < imageContainer->m_numMips
				, imageContainer->m_numLayers
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
				, flags
				, mem
			);
		}
		else if (1 < imageContainer->m_depth)
		{
			textureHandle = bgfx::createTexture3D(
				uint16_t(imageContainer->m_width)
				, uint16_t(imageContainer->m_height)
				, uint16_t(imageContainer->m_depth)
				, 1 < imageContainer->m_numMips
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
				, flags
				, mem
			);
		}
		else if (bgfx::isTextureValid(0, false, imageContainer->m_numLayers, bgfx::TextureFormat::Enum(imageContainer->m_format), flags))
		{
			textureHandle = bgfx::createTexture2D(
				uint16_t(imageContainer->m_width)
				, uint16_t(imageContainer->m_height)
				, 1 < imageContainer->m_numMips
				, imageContainer->m_numLayers
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
				, flags
				, mem
			);
		}

		if (!bgfx::isValid(textureHandle))
		{
			log::error("Failed to create texture '" + _path + "'!");
			return nullptr;
		}

		std::shared_ptr<TextureData> result = std::make_shared<TextureData>();
		result->init(textureHandle);
		result->width = (uint16_t)imageContainer->m_width;
		result->height = (uint16_t)imageContainer->m_height;
		bgfx::setName(textureHandle, _path.c_str());

		return result;
	}
	std::shared_ptr<AssetData> loadTexture(std::shared_ptr<TextureInputData> _input, TextureModes _textureModes)
	{
		if (_input == nullptr)
		{
			log::error("Null texture input data!");
			return nullptr;
		}
		return loadTexture(*_input.get(), _textureModes);
	}
	std::shared_ptr<AssetData> loadTexture(const TextureInputData& _input, TextureModes _textureModes)
	{
		if (_input.width == 0 || _input.height == 0)
		{
			log::error("Texture input data dimension is 0!");
			return nullptr;
		}

		const uint64_t flags = TextureModesToFlags(_textureModes);
		const bgfx::Memory* mem = bgfx::copy(_input.data.data(), uint32_t(_input.data.size()));
		const bool hasMips = false;
		const bool numLayers = 1;

		bgfx::TextureFormat::Enum textureFormat;
		size_t bytesPerPixel;
		if (_input.format == TextureFormat::RGBA8)
		{
			textureFormat = bgfx::TextureFormat::Enum::RGBA8;
			bytesPerPixel = 4;
		}
		else if (_input.format == TextureFormat::R8)
		{
			textureFormat = bgfx::TextureFormat::Enum::R8;
			bytesPerPixel = 1;
		}
		else
		{
			se::log::error("Texture::createResourceFromInput: Unknown texture format!");
			textureFormat = bgfx::TextureFormat::Enum::RGBA8;
			bytesPerPixel = 4;
		}
		se_assert(_input.data.size() % bytesPerPixel == 0);

		bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;
		if (_input.isCubemap)
		{
			se_assert(_input.width == _input.height);
			se_assert((_input.data.size() / bytesPerPixel) == (size_t)(_input.width * _input.height * 6));
			textureHandle = bgfx::createTextureCube(
				_input.width
				, hasMips
				, numLayers
				, textureFormat
				, flags
				, mem
			);
		}
		else
		{
			se_assert((_input.data.size() / bytesPerPixel) == (size_t)(_input.width * _input.height));
			textureHandle = bgfx::createTexture2D(
				_input.width
				, _input.height
				, hasMips
				, numLayers
				, textureFormat
				, flags
				, mem
			);
		}

		if (!bgfx::isValid(textureHandle))
		{
			log::error("Failed to create texture from input data!");
			return nullptr;
		}

		std::shared_ptr<TextureData> result = std::make_shared<TextureData>();
		result->init(textureHandle);
		result->width = _input.width;
		result->height = _input.height;
		bgfx::setName(textureHandle, "texture_from_input");

		return result;
	}
}