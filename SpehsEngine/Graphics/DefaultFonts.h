#pragma once

#include "SpehsEngine/Core/AssetManager.h"
#include "SpehsEngine/Graphics/Font.h"


namespace se::gfx
{
	enum class DefaultFontType
	{
		AnonymousPro_Regular,
		OpenSans_Regular,

		Count,

		Default = AnonymousPro_Regular
	};

	const char* getDefaultFontName(DefaultFontType _type = DefaultFontType::Default);
	std::shared_ptr<Font> createDefaultFont(DefaultFontType _type, AsyncTaskManager* _taskManager);
	void createDefaultFonts(AssetManager& _manager);
}
