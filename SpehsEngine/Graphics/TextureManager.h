#pragma once

#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/TextureInput.h"
#include "SpehsEngine/Graphics/ResourcePathFinder.h"
#include "SpehsEngine/Graphics/ResourceLoader.h"
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
			void setResourceLoader(ResourceLoader _resourceLoader);

			void update();
			void reloadTextures();
			void purgeUnusedTextures();
			bool allTexturesReady() const;

			std::shared_ptr<Texture> createTexture(const std::string_view _name, const std::string_view _texture, const TextureModes _textureModes = TextureModes());
			std::shared_ptr<Texture> createTexture(const std::string_view _name, const TextureInput& _input, const TextureModes _textureModes = TextureModes());
			std::shared_ptr<Texture> findTexture(const std::string_view _name) const;

		private:

			std::shared_ptr<ResourcePathFinder> pathFinder;
			ResourceLoader resourceLoader;

			std::vector<std::shared_ptr<Texture>> textures;
		};
	}
}
