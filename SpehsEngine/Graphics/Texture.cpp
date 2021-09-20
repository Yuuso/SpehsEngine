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
			: Resource(_name)
		{
		}
		Texture::~Texture()
		{
			destroy();
		}

		void Texture::reload(std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			if (!reloadable())
			{
				log::error("Cannot reload texture!");
				return;
			}
			destroy();
			create(path, textureModes, _resourceLoader);
		}
		bool Texture::reloadable() const
		{
			return !path.empty();
		}

		const uint16_t Texture::getWidth() const
		{
			se_assert_m(resourceData, "Texture not loaded!");
			if (!resourceData)
				return 0;
			return resourceData->info.width;
		}
		const uint16_t Texture::getHeight() const
		{
			se_assert_m(resourceData, "Texture not loaded!");
			if (!resourceData)
				return 0;
			return resourceData->info.height;
		}

		bool Texture::update()
		{
			const bool result = Resource::update();
			if (result)
			{
				setStatus(TextureStatus::Valid);
			}
			return result;
		}

		std::shared_ptr<ResourceData> Texture::createResource(const std::string _path, const TextureModes _textureModes)
		{
			static bx::DefaultAllocator defaultAllocator;
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

			std::shared_ptr<TextureData> result = std::make_shared<TextureData>();
			result->handle = textureHandle.idx;

			if (bgfx::isValid(textureHandle))
			{
				bgfx::setName(textureHandle, _path.c_str());

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
			}
			else
			{
				log::error("Failed to create texture '" + _path + "'!");
			}

			return result;
		}

		std::shared_ptr<ResourceData> Texture::createResourceFromInput(const TextureInput& _input, const TextureModes _textureModes)
		{
			se_assert(_input.format == TextureInput::Format::RGBA8); // Only one supported currently
			se_assert(_input.width > 0 && _input.height > 0);
			se_assert(_input.data.size() % 4 == 0);
			se_assert((_input.data.size() / 4) == (size_t)(_input.width * _input.height));

			const uint64_t flags = TextureModesToFlags(_textureModes);
			const bgfx::Memory* mem = bgfx::copy(_input.data.data(), uint32_t(_input.data.size()));

			bgfx::TextureHandle textureHandle = BGFX_INVALID_HANDLE;
			textureHandle = bgfx::createTexture2D(
				_input.width
				, _input.height
				, false
				, 1
				, bgfx::TextureFormat::Enum::RGBA8
				, flags
				, mem
				);

			std::shared_ptr<TextureData> result = std::make_shared<TextureData>();
			result->handle = textureHandle.idx;

			if (bgfx::isValid(textureHandle))
			{
				//bgfx::setName(textureHandle, name.c_str());

				bgfx::calcTextureSize(
					result->info
					, _input.width
					, _input.height
					, 1
					, false
					, 1
					, 1
					, bgfx::TextureFormat::Enum::RGBA8
					);
			}
			else
			{
				log::error("Failed to create texture from input!");
			}

			return result;
		}

		void Texture::create(const std::string_view _path, const TextureModes _textureModes, std::shared_ptr<ResourceLoader> _resourceLoader)
		{
			se_assert(status == TextureStatus::Init);

			path = _path;
			if (_resourceLoader)
			{
				std::function<std::shared_ptr<ResourceData>()> func = std::bind(&Texture::createResource, path, _textureModes);
				resourceFuture = _resourceLoader->push(func);
				setStatus(TextureStatus::Loading);
			}
			else
			{
				resourceData = std::dynamic_pointer_cast<TextureData>(createResource(path, _textureModes));
				setStatus(TextureStatus::Valid);
			}
		}

		void Texture::create(const TextureInput& _input, const TextureModes _textureModes)
		{
			se_assert(status == TextureStatus::Init);

			path.clear();
			resourceData = std::dynamic_pointer_cast<TextureData>(createResourceFromInput(_input, _textureModes));
			setStatus(TextureStatus::Valid);
		}

		void Texture::destroy()
		{
			if (status == TextureStatus::Valid)
			{
				ResourceHandle temp = getHandle();
				safeDestroy<bgfx::TextureHandle>(temp);
			}
			resourceData.reset();
			setStatus(TextureStatus::Init);
		}

		void Texture::setFallbacks(std::shared_ptr<TextureFallbacks> _fallbacks)
		{
			fallbacks = _fallbacks;
			setStatus(status);
		}

		void Texture::setStatus(const TextureStatus _status)
		{
			status = _status;
			if (!fallbacks)
				return;

			switch (status)
			{
				case TextureStatus::Init:
					resourceData = fallbacks->init;
					break;
				case TextureStatus::Loading:
					resourceData = fallbacks->loading;
					break;
				case TextureStatus::Valid:
					{
						bgfx::TextureHandle textureHandle = { getHandle() };
						if (!bgfx::isValid(textureHandle))
						{
							status = TextureStatus::Error;
							resourceData = fallbacks->error;
						}
					}
					break;
				case TextureStatus::Error:
					resourceData = fallbacks->error;
					break;
			}
		}
	}
}
