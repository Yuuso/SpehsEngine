#include "stdafx.h"
#include "SpehsEngine/Graphics/DefaultFonts.h"

#include "SpehsEngine/Core/EnumUtilityFunctions.h"
#include "SpehsEngine/Graphics/CharacterSet.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/OpenSans-Regular.ttf.h"
#include "SpehsEngine/Graphics/EmbeddedFonts/AnonymousPro-Regular.ttf.h"


namespace se::gfx
{
	static const uint8_t* getDefaultFontData(DefaultFontType _type)
	{
		switch (_type)
		{
			case DefaultFontType::Count: break;
			case DefaultFontType::AnonymousPro_Regular:	return font_anonymousProRegularTtf;
			case DefaultFontType::OpenSans_Regular:		return font_openSansRegularTtf;
		}
		log::error("Unknown DefaultFontType!");
		return nullptr;
	}
	static size_t getDefaultFontDataSize(DefaultFontType _type)
	{
		switch (_type)
		{
			case DefaultFontType::Count: break;
			case DefaultFontType::AnonymousPro_Regular:	return sizeof(font_anonymousProRegularTtf);
			case DefaultFontType::OpenSans_Regular:		return sizeof(font_openSansRegularTtf);
		}
		log::error("Unknown DefaultFontType!");
		return 0;
	}
	const char* getDefaultFontName(DefaultFontType _type)
	{
		switch (_type)
		{
			case DefaultFontType::Count: break;
			case DefaultFontType::AnonymousPro_Regular:	return "AnonymousPro-Regular";
			case DefaultFontType::OpenSans_Regular:		return "OpenSans-Regular";
		}
		log::error("Unknown DefaultFontType!");
		return "";
	}
	std::shared_ptr<Font> createDefaultFont(DefaultFontType _type, AsyncTaskManager* _taskManager)
	{
		static constexpr FontSize defaultFontSize(60, FontSizeType::Pixel);
		std::shared_ptr<Font> result = std::make_shared<Font>(getDefaultFontName(_type));
		result->load(
			_taskManager,
			getDefaultFontData(_type),
			getDefaultFontDataSize(_type),
			defaultFontSize,
			defaultCharacterSet);
		result->setPurgeProtection(true);
		return result;
	}

	void createDefaultFonts(AssetManager& _manager)
	{
		for (DefaultFontType type : enumRange(DefaultFontType::Count))
		{
			_manager.push(createDefaultFont(type, _manager.getAsyncTaskManager()));
		}
	}
}
