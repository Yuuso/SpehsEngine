#pragma once

#include "bgfx/bgfx.h" // ?!
#include "SpehsEngine/Graphics/Types.h"
#include <string>


namespace se
{
	namespace graphics
	{
		class Texture
		{
		public:

			Texture(const std::string_view _name);
			~Texture();

			Texture(const Texture& _other) = delete;
			Texture& operator=(const Texture& _other) = delete;

			Texture(Texture&& _other) = delete;
			Texture& operator=(Texture&& _other) = delete;


			void reload();

			const std::string& getName() const;

		private:

			friend class TextureManager;
			friend class Uniform;

			void create(const std::string_view _path, const TextureModes _textureModes);
			void destroy();

			const std::string name;
			std::string path;

			TextureModes textureModes;
			bgfx::TextureInfo info;
			bgfx::TextureHandle textureHandle;
		};
	}
}
