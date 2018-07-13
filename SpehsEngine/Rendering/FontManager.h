#pragma once
#include <vector>
#include <SpehsEngine/Core/Vector.h>

namespace se
{
	namespace rendering
	{
		class Font;
		class Window;

		class FontManager
		{
		public:

			Font* getFont(const std::string &_fontPath, const int &_size);

			Window& window;

		private:
			friend class Window;
			FontManager(Window& window);
			~FontManager();

			void* ftLibrary;
			std::vector<Font*> fonts;
		};
	}
}