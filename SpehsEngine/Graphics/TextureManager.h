#pragma once

#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Texture.h"
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

			TextureManager() = default;
			~TextureManager() = default;

			TextureManager(const TextureManager& _other) = delete;
			TextureManager& operator=(const TextureManager& _other) = delete;

			TextureManager(TextureManager&& _other) = delete;
			TextureManager& operator=(TextureManager&& _other) = delete;


			void reloadTextures();
			void purgeUnusedTextures();

			std::shared_ptr<Texture> createTexture(const std::string_view _name, const std::string_view _path, const TextureModes _textureModes = TextureModes());
			std::shared_ptr<Texture> findTexture(const std::string_view _name) const;

		private:

			std::vector<std::shared_ptr<Texture>> textures;
		};
	}
}
