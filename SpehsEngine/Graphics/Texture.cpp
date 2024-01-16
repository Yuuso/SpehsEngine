#include "stdafx.h"
#include "SpehsEngine/Graphics/Texture.h"

#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Graphics/Impl/AssetData.h"
#include "SpehsEngine/Graphics/Impl/TextureLoad.h"


Texture::Texture(std::string_view _name)
	: IAsset(_name)
{}
Texture::Texture(std::string_view _name, AsyncTaskManager* _taskManager,
		std::string_view _path, const TextureModes& _textureModes)
	: Texture(_name)
{
	load(_taskManager, _path, _textureModes);
}
Texture::Texture(std::string_view _name, AsyncTaskManager* _taskManager,
		TextureInput& _input, const TextureModes& _textureModes)
	: IAsset(_name)
{
	load(_taskManager, _input, _textureModes);
}
bool Texture::isReloadable() const
{
	return !path.empty();
}
void Texture::reload(AsyncTaskManager* _taskManager)
{
	if (!isReloadable())
	{
		log::warning("Texture " + getName() + " not reloadable!");
		return;
	}
	load(_taskManager, path, textureModes);
}
void Texture::load(AsyncTaskManager* _taskManager, std::string_view _path, const TextureModes& _textureModes)
{
	path = _path;
	textureModes = _textureModes;
	getImageDimensions(path, width, height);
	IAsset::load(_taskManager, [this]{ return loadTexture(path, textureModes); });
}
void Texture::load(AsyncTaskManager* _taskManager, TextureInput& _input, const TextureModes& _textureModes)
{
	path.clear();
	textureModes = _textureModes;
	std::shared_ptr<TextureInputData> inputData = _input.collectData();
	if (inputData)
	{
		width = inputData->width;
		height = inputData->height;
	}
	IAsset::load(_taskManager, [this, inputData]{ return loadTexture(inputData, textureModes); });
}
void Texture::load(const TextureInputData& _input, const TextureModes& _textureModes)
{
	path.clear();
	textureModes = _textureModes;
	// NOTE: This is always synchronous, so _input reference will be valid
	IAsset::load(nullptr, [this, &_input]{ return loadTexture(_input, textureModes); });
}
uint16_t Texture::getWidth() const
{
	if (width == 0)
	{
		log::error("Texture width not initialized!");
	}
	return width;
}
uint16_t Texture::getHeight() const
{
	if (height == 0)
	{
		log::error("Texture height not initialized!");
	}
	return height;
}
const std::string& Texture::getPath() const
{
	return path;
}
const TextureModes& Texture::getTextureModes() const
{
	return textureModes;
}
void Texture::handleLoaded()
{
	if (!isValid())
		return;
	const TextureData* ptr = getData<TextureData>();
	width = ptr->width;
	height = ptr->height;
	se_assert(width > 0 && height > 0);
}
