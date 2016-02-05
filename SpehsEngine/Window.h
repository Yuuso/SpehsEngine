#pragma once
#include <string>

struct SDL_Window;
namespace SpehsEngine
{
	enum SpehsEngine_WindowFlags
	{
		FULLSCREEN = 0x1,
		BORDERLESS = 0x2,
	};

	class Window
	{
	public:
		Window();
		~Window();

		int create(std::string windowName, int scrWidth, int scrHeight, unsigned int currentFlags);
		void swapBuffers();
		void clearBuffer();
		void clearColor(float _r, float _g, float _b, float _a);

	private:
		SDL_Window* sdlWindow;
	};
}
extern SpehsEngine::Window* mainWindow;