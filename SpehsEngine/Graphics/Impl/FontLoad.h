#pragma once

#include "SpehsEngine/Graphics/CharacterSet.h"


namespace se::gfx::impl
{
	std::shared_ptr<AssetData> loadFont(std::string _path, FontSize _size, CharacterSet _charSet);
	std::shared_ptr<AssetData> loadFont(const uint8_t* _data, size_t _dataSize, FontSize _size, CharacterSet _charSet);
}
