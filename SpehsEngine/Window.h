#pragma once
#include <string>

struct SDL_Window;
namespace spehs
{
	enum SpehsEngine_WindowFlags
	{
		FULLSCREEN = 0x1,
		BORDERLESS = 0x2,
	};

	class Window
	{
		friend class InputManager;
	public:
		Window();
		~Window();

		int create(std::string windowName, int scrWidth, int scrHeight, unsigned int currentFlags);
		void renderBegin();
		void renderEnd();
		void clearColor(float _r, float _g, float _b, float _a);

	private:
		SDL_Window* sdlWindow;
	};
}
extern spehs::Window* mainWindow;