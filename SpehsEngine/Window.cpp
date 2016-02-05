#include "Window.h"
#include "Exceptions.h"
#include "ApplicationData.h"


SpehsEngine::Window* mainWindow;
namespace SpehsEngine
{
	Window::Window()
	{
	}
	Window::~Window()
	{
	}

	int Window::create(std::string windowName, int scrWidth, int scrHeight, unsigned int currentFlags)
	{
		Uint32 flags = SDL_WINDOW_OPENGL;

		if (currentFlags & FULLSCREEN)
			flags |= SDL_WINDOW_FULLSCREEN;
		if (currentFlags & BORDERLESS)
			flags |= SDL_WINDOW_BORDERLESS;

		sdlWindow = SDL_CreateWindow(windowName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, scrWidth, scrHeight, flags);
		if (sdlWindow == nullptr)
			fatalError("\nFailed to create window!");

		//Update window size if it changed
		
		int w, h;
		SDL_GetWindowSize(sdlWindow, &w, &h);
		applicationData->setWindowWidth(w);
		applicationData->setWindowHeight(h);

		SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
		if (glContext == nullptr)
			fatalError("\nFailed to create glContext!");

		GLenum error = glewInit();
		if (error != GLEW_OK)
			fatalError("\nFailed to initialize glew!");

		//Check OpenGL version
		std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
		//Set VSync
		SDL_GL_SetSwapInterval(0);//0 disable, 1 enable, -1 ???

		//Enable alpha blending
		glEnable(GL_BLEND);
		//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//MSAA Antialiasing
		glEnable(GL_MULTISAMPLE);

		return 0;
	}


	void Window::swapBuffers()
	{
		SDL_GL_SwapWindow(sdlWindow);
	}


	void Window::clearBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}


	void Window::clearColor(float _r, float _g, float _b, float _a)
	{
		glClearColor(_r, _g, _b, _a);
	}
}