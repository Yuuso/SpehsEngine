#pragma once

#include "SpehsEngine/Core/Asset.h"
#include "SpehsEngine/Graphics/TextureInput.h"


namespace se::gfx
{
	class Texture : public IAsset
	{
	public:

		~Texture() = default;

		// Construct empty texture
		Texture(std::string_view _name);

		// Construct and load from file
		Texture(std::string_view _name, AsyncTaskManager* _taskManager,
			std::string_view _path, const TextureModes& _textureModes = TextureModes{});

		// Construct and load from input data
		Texture(std::string_view _name, AsyncTaskManager* _taskManager,
			TextureInput& _input, const TextureModes& _textureModes = TextureModes{});

		bool isReloadable() const override;
		void reload(AsyncTaskManager* _taskManager) override;

		// Load texture from file
		void load(AsyncTaskManager* _taskManager, std::string_view _path, const TextureModes& _textureModes);

		// Load texture from input data
		void load(AsyncTaskManager* _taskManager, TextureInput& _input, const TextureModes& _textureModes);

		// Load texture synchronously from input data
		void load(const TextureInputData& _input, const TextureModes& _textureModes);

		// Texture dimensions
		uint16_t getWidth() const;
		uint16_t getHeight() const;

		const std::string& getPath() const;
		const TextureModes& getTextureModes() const;

	private:

		void handleLoaded() override;

		std::string path;
		TextureModes textureModes;
		uint16_t width = 0;
		uint16_t height = 0;
	};
}
