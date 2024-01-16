#pragma once

#include "SpehsEngine/Core/Asset.h"
#include "SpehsEngine/Graphics/CharacterSet.h"


namespace se::gfx
{
	class Font : public IAsset
	{
	public:

		~Font() = default;

		// Construct empty font
		Font(std::string_view _name);

		// Construct and load font from file
		Font(std::string_view _name, AsyncTaskManager* _taskManager,
			std::string_view _path, FontSize _size = FontSize{}, const CharacterSet& _charSet = defaultCharacterSet);

		// Construct and load font from data, see load function for notes!
		Font(std::string_view _name, AsyncTaskManager* _taskManager,
			const uint8_t* _data, size_t _dataSize, FontSize _size = FontSize{}, const CharacterSet& _charSet = defaultCharacterSet);

		bool isReloadable() const override;
		void reload(AsyncTaskManager* _taskManager) override;

		// Load font from file
		void load(AsyncTaskManager* _taskManager,
			std::string_view _path, FontSize _size, const CharacterSet& _charSet);

		// Load from from data
		//	- Intended for embedded fonts
		//	- Data needs to be valid until loading is finished!
		void load(AsyncTaskManager* _taskManager,
			const uint8_t* _data, size_t _dataSize, FontSize _size, const CharacterSet& _charSet);

		const std::string& getPath() const;
		FontSize getFontSize() const;
		const CharacterSet& getCharacterSet() const;

	private:

		std::string path;
		FontSize size;
		CharacterSet charSet;
	};
}
