#pragma once

#include "SpehsEngine/Graphics/Internal/ResourceManager.h"
#include "SpehsEngine/Graphics/Internal/TextureFallbacks.h"
#include "SpehsEngine/Graphics/Types.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Graphics/TextureInput.h"
#include <memory>
#include <string>


namespace se
{
	namespace graphics
	{
		class TextureManager : public ResourceManager<Texture>
		{
		public:

			TextureManager();
			~TextureManager();

			TextureManager(const TextureManager& _other) = delete;
			TextureManager& operator=(const TextureManager& _other) = delete;

			TextureManager(TextureManager&& _other) = delete;
			TextureManager& operator=(TextureManager&& _other) = delete;


			std::shared_ptr<Texture> create(const std::string_view _name, const std::string_view _texture, const TextureModes _textureModes = TextureModes());
			std::shared_ptr<Texture> create(const std::string_view _name, const TextureInput& _input, const TextureModes _textureModes = TextureModes());
			std::shared_ptr<Texture> find(const std::string_view _name) const;
			void remove(std::string_view _name);

		private:

			void initFallbacks();
			void destroyFallbacks();

			std::shared_ptr<TextureFallbacks> fallbacks;
		};
	}
}
