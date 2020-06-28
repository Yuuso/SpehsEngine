#include "stdafx.h"
#include "SpehsEngine/Graphics/TextureManager.h"

#include "SpehsEngine/Core/StringOperations.h"


namespace se
{
	namespace graphics
	{
		TextureManager::TextureManager()
		{
		}
		TextureManager::~TextureManager()
		{
		}


		void TextureManager::reloadTextures()
		{
			se_assert(false);
			// TODO
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

		std::shared_ptr<Texture> TextureManager::createTexture(const std::string_view _name, const std::string_view _path, const TextureModes _textureModes)
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
				se_assert(foundTexture->path == _path);
				return foundTexture;
			}

			std::shared_ptr<Texture>& texture = textures.emplace_back(std::make_shared<Texture>(_name));
			texture->create(_path, _textureModes);
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
