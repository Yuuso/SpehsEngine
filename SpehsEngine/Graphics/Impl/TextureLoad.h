#pragma once

#include "SpehsEngine/Graphics/TextureInput.h"


namespace se::gfx::impl
{
	bool getImageDimensions(const std::string& _path, uint16_t& _width, uint16_t& _height);
	std::shared_ptr<AssetData> loadTexture(std::string _path, TextureModes _textureModes);
	std::shared_ptr<AssetData> loadTexture(std::shared_ptr<TextureInputData> _input, TextureModes _textureModes);
	std::shared_ptr<AssetData> loadTexture(const TextureInputData& _input, TextureModes _textureModes);
}
