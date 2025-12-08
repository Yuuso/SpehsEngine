#pragma once

#include "SpehsEngine/Graphics/Resource.h"


namespace se
{
	namespace graphics
	{
		struct TextureFallbacks;
		struct ResourceData;
		enum class TextureStatus;

		class Texture final : public Resource<TextureData>
		{
		public:

			Texture(const std::string_view _name);
			~Texture();

			Texture(const Texture& _other) = delete;
			Texture& operator=(const Texture& _other) = delete;

			Texture(Texture&& _other) = delete;
			Texture& operator=(Texture&& _other) = delete;


			void reload(std::shared_ptr<ResourceLoader> _resourceLoader = nullptr) override;
			bool reloadable() const;
			bool update() override;

			void setTextureData(const uint16_t _layer, const uint16_t _x, const uint16_t _y, const uint16_t _width, const uint16_t _height, const std::vector<uint8_t>& _data);

			uint16_t getWidth() const;
			uint16_t getHeight() const;
			uint16_t getLayerCount() const;
			TextureFormat getTextureFormat() const;
			bool getTextureDataMutable() const;

		private:

			friend class TextureManager;
			friend class Font;

			static std::shared_ptr<ResourceData> createResource(const std::string _path, const TextureModes _textureModes);
			static std::shared_ptr<ResourceData> createResourceFromInput(const TextureInput& _input, const TextureModes _textureModes);
			void create(const std::string_view _path, const TextureModes _textureModes, std::shared_ptr<ResourceLoader> _resourceLoader);
			void create(const TextureInput& _input, const TextureModes _textureModes);
			void destroy();
			void setFallbacks(std::shared_ptr<TextureFallbacks> _fallbacks);
			void setStatus(const TextureStatus _status);

			std::string path;

			TextureModes textureModes;
			TextureStatus status;
			std::shared_ptr<TextureFallbacks> fallbacks;
		};
	}
}
