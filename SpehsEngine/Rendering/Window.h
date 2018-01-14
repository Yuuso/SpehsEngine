#pragma once
#include <boost/signal.hpp>

struct SDL_Window;
namespace spehs
{
	class InputManager;
	class GLContext;

	class Window
	{
	public:
		friend class InputManager;
		friend class GLContext;
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

		/* Returns window pixel width. */
		int getWidth() const;
		/* Returns window pixel height. */
		int getHeight() const;

		/* Fires when the size of the window is changed. */
		boost::signal<void(const int, const int)> sizeChangedSignal;

	private:
		SDL_Window* sdlWindow;
		void* glContext;
		int width;
		int height;
	};
}