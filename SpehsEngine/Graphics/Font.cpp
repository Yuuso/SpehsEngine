#include "stdafx.h"
#include "SpehsEngine/Graphics/Font.h"

#include "SpehsEngine/Core/AsyncTaskManager.h"
#include "SpehsEngine/Core/File/File.h"
#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/Impl/AssetData.h"
#include "SpehsEngine/Graphics/Impl/FontLoad.h"


Font::Font(std::string_view _name)
	: IAsset(_name)
{}
Font::Font(std::string_view _name, AsyncTaskManager* _taskManager,
		std::string_view _path, FontSize _size, const CharacterSet& _charSet)
	: Font(_name)
{
	load(_taskManager, _path, _size, _charSet);
}
Font::Font(std::string_view _name, AsyncTaskManager* _taskManager,
		const uint8_t* _data, size_t _dataSize, FontSize _size, const CharacterSet& _charSet)
	: Font(_name)
{
	load(_taskManager, _data, _dataSize, _size, _charSet);
}
bool Font::isReloadable() const
{
	return !path.empty();
}
void Font::reload(AsyncTaskManager* _taskManager)
{
	if (!isReloadable())
	{
		log::warning("Font " + getName() + " not reloadable!");
		return;
	}
	load(_taskManager, path, size, charSet);
}
void Font::load(AsyncTaskManager* _taskManager,
	std::string_view _path, FontSize _size, const CharacterSet& _charSet)
{
	path = _path;
	size = _size;
	charSet = _charSet;
	IAsset::load(_taskManager, [this]{ return loadFont(path, size, charSet); });
}
void Font::load(AsyncTaskManager* _taskManager,
	const uint8_t* _data, size_t _dataSize, FontSize _size, const CharacterSet& _charSet)
{
	path.clear();
	size = _size;
	charSet = _charSet;
	IAsset::load(_taskManager, [this, _data, _dataSize]{ return loadFont(_data, _dataSize, size, charSet); });
}
const std::string& Font::getPath() const
{
	return path;
}
FontSize Font::getFontSize() const
{
	return size;
}
const CharacterSet& Font::getCharacterSet() const
{
	return charSet;
}
