#include "stdafx.h"
#include "SpehsEngine/Graphics/TextureManager.h"

#include "SpehsEngine/Core/StringViewUtilityFunctions.h"
#include "SpehsEngine/Graphics/Internal/InternalUtilities.h"


namespace se
{
	namespace graphics
	{
		TextureManager::TextureManager()
			: ResourceManager()
		{
			initFallbacks();
		}
		TextureManager::~TextureManager()
		{
			destroyFallbacks();
		}

		void TextureManager::initFallbacks()
		{
			fallbacks = std::make_shared<TextureFallbacks>();

			TextureModes modes;
			modes.sampleMin = se::graphics::TextureSamplingMode::Point;
			modes.sampleMag = se::graphics::TextureSamplingMode::Point;
			modes.sampleMip = se::graphics::TextureSamplingMode::Point;

			TextureInput input;
			input.format = TextureInput::Format::RGBA8;
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
							255,	0,		255,	255 };
			fallbacks->init = std::dynamic_pointer_cast<TextureData>(Texture::createResourceFromInput(input, modes));

			input.data = {	255,	0,		255,	255,
							0,		255,	0,		255,
							255,	0,		255,	255,

							0,		255,	0,		255,
							255,	0,		255,	255,
							0,		255,	0,		255,

							255,	0,		255,	255,
							0,		255,	0,		255,
							255,	0,		255,	255 };
			fallbacks->loading = std::dynamic_pointer_cast<TextureData>(Texture::createResourceFromInput(input, modes));

			input.data = {	255,	0,		255,	255,
							255,	0,		255,	255,
							255,	0,		255,	255,

							255,	0,		255,	255,
							255,	0,		255,	255,
							255,	0,		255,	255,

							255,	0,		255,	255,
							255,	0,		255,	255,
							255,	0,		255,	255 };
			fallbacks->error = std::dynamic_pointer_cast<TextureData>(Texture::createResourceFromInput(input, modes));
		}
		void TextureManager::destroyFallbacks()
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

		std::shared_ptr<Texture> TextureManager::create(const std::string_view _name, const std::string_view _texture, const TextureModes _textureModes)
		{
			const std::string path = pathFinder->getPath(_texture);

			for (auto& texture : resources)
			{
				if (texture->getName() == _name)
				{
					log::warning("Cannot create texture '" + _name + "', texture with that name already exists!");
					if (texture->path != path)
						log::error("Existing texture's '" + _name + "' resource path doesn't match!");
					return texture;
				}
			}

			resources.push_back(std::make_shared<Texture>(_name));
			std::shared_ptr<Texture>& texture = resources.back();
			texture->setFallbacks(fallbacks);
			texture->create(path, _textureModes, resourceLoader);
			return texture;
		}
		std::shared_ptr<Texture> TextureManager::create(const std::string_view _name, const TextureInput& _input, const TextureModes _textureModes)
		{
			for (auto& texture : resources)
			{
				if (texture->getName() == _name)
				{
					log::warning("Cannot create texture '" + _name + "', texture with that name already exists!");
					return texture;
				}
			}

			resources.push_back(std::make_shared<Texture>(_name));
			std::shared_ptr<Texture>& texture = resources.back();
			texture->setFallbacks(fallbacks);
			texture->create(_input, _textureModes);
			return texture;
		}
		std::shared_ptr<Texture> TextureManager::find(const std::string_view _name) const
		{
			for (auto& texture : resources)
			{
				if (texture->getName() == _name)
					return texture;
			}
			return nullptr;
		}
	}
}
