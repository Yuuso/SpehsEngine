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

		void Texture::reload(ResourceLoader _resourceLoader)
		{
			if (path.empty())
			{
				log::error("Cannot reload texture, path is empty!");
				return;
			}
			destroy();
			create(path, textureModes, _resourceLoader);
		}

		const std::string& Texture::getName() const
		{
			return name;
		}

		std::shared_ptr<ResourceData> Texture::createResource(const std::string _path, const TextureModes _textureModes)
		{
			static bx::DefaultAllocator defaultAllocator;
			std::ifstream input(_path, std::ios::binary);
			if (!input.is_open())
			{
				log::error("Failed to open texture: " + _path);
				return nullptr;
			}

			std::vector<uint8_t> data(std::istreambuf_iterator<char>(input), {});
			if (data.empty())
			{
				log::error("Failed to read texture data: " + _path);
				return nullptr;
			}

			bimg::ImageContainer* imageContainer = bimg::imageParse(&defaultAllocator, data.data(), static_cast<uint32_t>(data.size()));
			input.close();
			if (imageContainer == nullptr)
			{
				log::error("Failed to parse texture: " + _path);
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
				bgfx::setName(textureHandle, _path.c_str());
			}

			std::shared_ptr<TextureData> result = std::make_shared<TextureData>();
			result->handle = textureHandle.idx;

			bgfx::calcTextureSize(
				result->info
				, uint16_t(imageContainer->m_width)
				, uint16_t(imageContainer->m_height)
				, uint16_t(imageContainer->m_depth)
				, imageContainer->m_cubeMap
				, 1 < imageContainer->m_numMips
				, imageContainer->m_numLayers
				, bgfx::TextureFormat::Enum(imageContainer->m_format)
				);

			return result;
		}

		void Texture::create(const std::string_view _path, const TextureModes _textureModes, ResourceLoader _resourceLoader)
		{
			se_assert(!resourceData);

			path = _path;
			if (_resourceLoader)
			{
				std::function<std::shared_ptr<ResourceData>()> func = std::bind(&Texture::createResource, path, _textureModes);
				resourceFuture = _resourceLoader->push(func);
				// TODO set temp texture handle
			}
			else
			{
				resourceData = std::dynamic_pointer_cast<TextureData>(createResource(path, _textureModes));
			}
		}

		bgfx::TextureHandle Texture::getHandle() const
		{
			assert(resourceData);

			return { resourceData->handle };
		}

		void Texture::destroy()
		{
			assert(resourceData);

			bgfx::TextureHandle textureHandle = getHandle();
			if (bgfx::isValid(textureHandle))
			{
				bgfx::destroy(textureHandle);
				resourceData.reset();
			}
		}
	}
}
