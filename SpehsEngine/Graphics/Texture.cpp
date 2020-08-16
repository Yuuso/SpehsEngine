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

		void Texture::reload(std::shared_ptr<AsyncTaskManager<ResourceHandle>> _resourceLoader)
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

		ResourceHandle Texture::createResource(const std::string _path, const TextureModes _textureModes)
		{
			static bx::DefaultAllocator defaultAllocator;
			std::ifstream input(_path, std::ios::binary);
			if (!input.is_open())
			{
				log::error("Failed to open texture: " + _path);
				return bgfx::kInvalidHandle;
			}

			std::vector<uint8_t> data(std::istreambuf_iterator<char>(input), {});
			if (data.empty())
			{
				log::error("Failed to read texture data: " + _path);
				return bgfx::kInvalidHandle;
			}

			bimg::ImageContainer* imageContainer = bimg::imageParse(&defaultAllocator, data.data(), static_cast<uint32_t>(data.size()));
			if (imageContainer == nullptr)
			{
				log::error("Failed to parse texture: " + _path);
				return bgfx::kInvalidHandle;
			}

			// TODO: bimg::Orientation::Enum ; imageContainer->m_orientation;

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

			input.close();
			return textureHandle.idx;
		}

		void Texture::create(const std::string_view _path, const TextureModes _textureModes, std::shared_ptr<AsyncTaskManager<ResourceHandle>> _resourceLoader)
		{
			se_assert(resourceHandle == INVALID_RESOURCE_HANDLE);

			path = _path;
			if (_resourceLoader)
			{
				std::function<ResourceHandle()> func = std::bind(&Texture::createResource, path, _textureModes);
				resourceFuture = _resourceLoader->push(func);
				// TODO set temp texture handle
			}
			else
			{
				resourceHandle = createResource(path, _textureModes);
			}
		}

		bgfx::TextureHandle Texture::getHandle() const
		{
			return { resourceHandle };
		}

		void Texture::destroy()
		{
			bgfx::TextureHandle textureHandle = getHandle();
			if (bgfx::isValid(textureHandle))
			{
				bgfx::destroy(textureHandle);
				textureHandle = BGFX_INVALID_HANDLE;
			}
		}
	}
}
