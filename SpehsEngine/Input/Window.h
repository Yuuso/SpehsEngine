
#pragma once

struct SDL_Window;
namespace spehs
{
	namespace input
	{
		class Window
		{
		public:
			friend class InputManager;
		public:

			Window(const int pixelWidth, const int pixelHeight);
			~Window();
			
			void renderBegin();
			void renderEnd();

			void setClearColor(const float _r, const float _g, const float _b, const float _a);
			void setTitle(const char* title);
			void setSize(const int pixelWidth, const int pixelHeight);
			void setMinSize(const int minPixelWidth, const int minPixelHeight);
			void setMaxSize(const int maxPixelWidth, const int maxPixelHeight);
			void setFullscreen(const bool enabled);
			void setBorderless(const bool enabled);

		private:
			SDL_Window* sdlWindow;
		};
	}
}