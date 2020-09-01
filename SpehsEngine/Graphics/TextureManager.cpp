#include "stdafx.h"
#include "SpehsEngine/Graphics/TextureManager.h"

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"


namespace se
{
	namespace graphics
	{
		TextureManager::TextureManager()
		{
			pathFinder = std::make_shared<ResourcePathFinder>();

			fallbacks = std::make_shared<TextureFallbacks>();
			{
				TextureModes modes;
				modes.sampleMin = se::graphics::TextureSamplingMode::Point;
				modes.sampleMag = se::graphics::TextureSamplingMode::Point;
				modes.sampleMip = se::graphics::TextureSamplingMode::Point;

				TextureInput input;
				input.width = 3;
				input.height = 3;

				input.data = {	255,	0,		255,	255,
								255,	255,	255,	255,
								255,	0,		255,	255,

								255,	255,	255,	255,
								255,	0,		255,	255,
								255,	255,	255,	255,

								255,	0,		255,	255,
								255,	255,	255,	255,
								255,	0,		255,	255		};
				fallbacks->init = std::dynamic_pointer_cast<TextureData>(Texture::createResourceFromInput(input, modes));

				input.data = {	255,	0,		255,	255,
								0,		255,	0,		255,
								255,	0,		255,	255,

								0,		255,	0,		255,
								255,	0,		255,	255,
								0,		255,	0,		255,

								255,	0,		255,	255,
								0,		255,	0,		255,
								255,	0,		255,	255		};
				fallbacks->loading = std::dynamic_pointer_cast<TextureData>(Texture::createResourceFromInput(input, modes));

				input.data = {	255,	0,		255,	255,
								255,	0,		255,	255,
								255,	0,		255,	255,

								255,	0,		255,	255,
								255,	0,		255,	255,
								255,	0,		255,	255,

								255,	0,		255,	255,
								255,	0,		255,	255,
								255,	0,		255,	255		};
				fallbacks->error = std::dynamic_pointer_cast<TextureData>(Texture::createResourceFromInput(input, modes));
			}
		}

		TextureManager::~TextureManager()
		{
			if (fallbacks)
			{
				if (fallbacks->init)
					safeDestroy<bgfx::TextureHandle>(fallbacks->init->handle);
				if (fallbacks->loading)
					safeDestroy<bgfx::TextureHandle>(fallbacks->loading->handle);
				if (fallbacks->error)
					safeDestroy<bgfx::TextureHandle>(fallbacks->error->handle);
			}
		}

		void TextureManager::setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder)
		{
			pathFinder = _pathFinder;
		}
		void TextureManager::setResourceLoader(ResourceLoader _resourceLoader)
		{
			resourceLoader = _resourceLoader;
		}

		void TextureManager::update()
		{
			for (auto&& texture : textures)
			{
				texture->update();
			}
		}

		void TextureManager::reloadTextures()
		{
			for (auto&& texture : textures)
			{
				if (!texture->path.empty())
					texture->reload(resourceLoader);
			}
		}

		void TextureManager::purgeUnusedTextures()
		{
			for (size_t i = 0; i < textures.size(); /*i++*/)
			{
				if (textures[i].use_count() == 1)
				{
					textures[i].swap(textures.back());
					textures.pop_back();
				}
				else
				{
					i++;
				}
			}
		}

		bool TextureManager::allTexturesReady() const
		{
			for (auto&& texture : textures)
			{
				if (!texture->ready())
				{
					return false;
				}
			}
			return true;
		}

		std::shared_ptr<Texture> TextureManager::createTexture(const std::string_view _name, const std::string_view _texture, const TextureModes _textureModes)
		{
			const std::string path = pathFinder->getPath(_texture);

			std::shared_ptr<Texture> foundTexture;
			for (auto& texture : textures)
			{
				if (texture->getName() == _name)
					foundTexture = texture;
			}
			if (foundTexture)
			{
				log::warning("Cannot create texture '" + _name + "', texture with that name already exists!");
				se_assert(foundTexture->path == path);
				return foundTexture;
			}

			std::shared_ptr<Texture>& texture = textures.emplace_back(std::make_shared<Texture>(_name));
			texture->setFallbacks(fallbacks);
			texture->create(path, _textureModes, resourceLoader);
			return texture;
		}
		std::shared_ptr<Texture> TextureManager::createTexture(const std::string_view _name, const TextureInput& _input, const TextureModes _textureModes)
		{
			std::shared_ptr<Texture> foundTexture;
			for (auto& texture : textures)
			{
				if (texture->getName() == _name)
					foundTexture = texture;
			}
			if (foundTexture)
			{
				log::warning("Cannot create texture '" + _name + "', texture with that name already exists!");
				return foundTexture;
			}

			std::shared_ptr<Texture>& texture = textures.emplace_back(std::make_shared<Texture>(_name));
			texture->setFallbacks(fallbacks);
			texture->create(_input, _textureModes);
			return texture;
		}
		std::shared_ptr<Texture> TextureManager::findTexture(const std::string_view _name) const
		{
			for (auto& texture : textures)
			{
				if (texture->getName() == _name)
					return texture;
			}
			se_assert_m(false, "Texture '" + std::string(_name) + "' not found!");
			return nullptr;
		}
	}
}
