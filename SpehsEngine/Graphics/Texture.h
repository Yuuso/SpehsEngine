#pragma once

#include "bgfx/bgfx.h" // ?!
#include "SpehsEngine/Graphics/Internal/Resource.h"
#include "SpehsEngine/Graphics/Types.h"
#include <string>


namespace se
{
	namespace graphics
	{
		class Texture final : public Resource
		{
		public:

			Texture(const std::string_view _name);
			~Texture();

			Texture(const Texture& _other) = delete;
			Texture& operator=(const Texture& _other) = delete;

			Texture(Texture&& _other) = delete;
			Texture& operator=(Texture&& _other) = delete;


			void reload(std::shared_ptr<AsyncTaskManager<ResourceHandle>> _resourceLoader);

			const std::string& getName() const;

		private:

			friend class TextureManager;
			friend class Uniform;

			static ResourceHandle createResource(const std::string _path, const TextureModes _textureModes);
			void create(const std::string_view _path, const TextureModes _textureModes, std::shared_ptr<AsyncTaskManager<ResourceHandle>> _resourceLoader);
			void destroy();

			bgfx::TextureHandle getHandle() const;

			const std::string name;
			std::string path;

			TextureModes textureModes;
			//bgfx::TextureInfo info;
		};
	}
}
