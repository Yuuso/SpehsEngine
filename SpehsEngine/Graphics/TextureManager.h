#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/ResourcePathFinder.h"
#include <memory>
#include <string>
#include <vector>


namespace se
{
	namespace graphics
	{
		class TextureManager
		{
		public:

			TextureManager();
			~TextureManager() = default;

			TextureManager(const TextureManager& _other) = delete;
			TextureManager& operator=(const TextureManager& _other) = delete;

			TextureManager(TextureManager&& _other) = delete;
			TextureManager& operator=(TextureManager&& _other) = delete;


			void setResourcePathFinder(std::shared_ptr<ResourcePathFinder> _pathFinder);

			void reloadTextures();
			void purgeUnusedTextures();

			std::shared_ptr<Texture> createTexture(const std::string_view _name, const std::string_view _texture, const TextureModes _textureModes = TextureModes());
			std::shared_ptr<Texture> findTexture(const std::string_view _name) const;

		private:

			std::shared_ptr<ResourcePathFinder> pathFinder;

			std::vector<std::shared_ptr<Texture>> textures;
		};
	}
}
