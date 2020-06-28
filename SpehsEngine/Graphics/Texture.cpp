#include "stdafx.h"
#include "SpehsEngine/Graphics/Texture.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"
#include "bx/allocator.h"
#include "bimg/bimg.h"
#include "bimg/decode.h"
#include <fstream>
#include <iterator>


namespace se
{
	namespace graphics
	{
		Texture::Texture(const std::string_view _name)
			: name(_name)
		{
		}
		Texture::~Texture()
		{
			destroy();
		}

		void Texture::reload()
		{
			if (path.empty())
			{
				log::error("Cannot reload texture, path is empty!");
				return;
			}
			destroy();
			create(path, textureModes);
		}

		const std::string& Texture::getName() const
		{
			return name;
		}

		void Texture::create(const std::string_view _path, const TextureModes _textureModes)
		{
			static bx::DefaultAllocator defaultAllocator;
			path = _path;
			std::ifstream input(path, std::ios::binary);
			if (!input.is_open())
			{
				log::error("Failed to open texture: " + path);
				return;
			}

			std::vector<uint8_t> data(std::istreambuf_iterator<char>(input), {});
			if (data.empty())
			{
				log::error("Failed to read texture data: " + path);
				return;
			}

			bimg::ImageContainer* imageContainer = bimg::imageParse(&defaultAllocator, data.data(), static_cast<uint32_t>(data.size()));
			if (imageContainer == nullptr)
			{
				log::error("Failed to parse texture: " + path);
				return;
			}

			// TODO: bimg::Orientation::Enum ; imageContainer->m_orientation;

			auto imageReleaseCb = []([[maybe_unused]] void* _ptr, void* _userData)
				{
					bimg::ImageContainer* imageContainer = (bimg::ImageContainer*)_userData;
					bimg::imageFree(imageContainer);
				};

			const bgfx::Memory* mem = bgfx::makeRef(imageContainer->m_data, imageContainer->m_size, imageReleaseCb, imageContainer);
			const uint64_t flags = TextureModesToFlags(_textureModes);

			if (imageContainer->m_cubeMap)
			{
				textureHandle = bgfx::createTextureCube(
					uint16_t(imageContainer->m_width),
					1 < imageContainer->m_numMips,
					imageContainer->m_numLayers,
					bgfx::TextureFormat::Enum(imageContainer->m_format),
					flags,
					mem);
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

			if (bgfx::isValid(textureHandle))
			{
				bgfx::setName(textureHandle, path.c_str());
			}

			// TODO
			//if (NULL != _info)
			//{
			//	bgfx::calcTextureSize(
			//		*_info
			//		, uint16_t(imageContainer->m_width)
			//		, uint16_t(imageContainer->m_height)
			//		, uint16_t(imageContainer->m_depth)
			//		, imageContainer->m_cubeMap
			//		, 1 < imageContainer->m_numMips
			//		, imageContainer->m_numLayers
			//		, bgfx::TextureFormat::Enum(imageContainer->m_format)
			//	);
			//}
		}
		void Texture::destroy()
		{
			if (bgfx::isValid(textureHandle))
			{
				bgfx::destroy(textureHandle);
				textureHandle = BGFX_INVALID_HANDLE;
			}
		}
	}
}
