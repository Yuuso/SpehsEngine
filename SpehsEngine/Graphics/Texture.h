#pragma once

#include "bgfx/bgfx.h" // ?!
#include "SpehsEngine/Graphics/Internal/Resource.h"
#include "SpehsEngine/Graphics/ResourceData.h"
#include "SpehsEngine/Graphics/TextureInput.h"
#include "SpehsEngine/Graphics/Types.h"
#include <string>


namespace se
{
	namespace graphics
	{
		struct TextureData : ResourceData
		{
			bgfx::TextureInfo info;
		};

		class Texture final : public Resource<TextureData>
		{
		public:

			Texture(const std::string_view _name);
			~Texture();

			Texture(const Texture& _other) = delete;
			Texture& operator=(const Texture& _other) = delete;

			Texture(Texture&& _other) = delete;
			Texture& operator=(Texture&& _other) = delete;


			void reload(ResourceLoader _resourceLoader = nullptr) override;

			const std::string& getName() const;

		private:

			friend class TextureManager;
			friend class Uniform;

			static std::shared_ptr<ResourceData> createResource(const std::string _path, const TextureModes _textureModes);
			void create(const std::string_view _path, const TextureModes _textureModes, ResourceLoader _resourceLoader);
			void create(const TextureInput& _input, const TextureModes _textureModes);
			void destroy();

			const std::string name;
			std::string path;

			TextureModes textureModes;
		};
	}
}
