#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Log.h"
//#include "SpehsEngine/Rendering/PostProcessing.h" TODO!!!

#include <SDL/SDL_video.h>
#include <GL/glew.h>

namespace spehs
{
	namespace graphics
	{
		namespace postproc
		{
			extern void postProcessingBegin();
			extern void postProcessingEnd();
		}
	}

	Window::Window(const int pixelWidth, const int pixelHeight)
		: sdlWindow(nullptr)
		, glContext(nullptr)
		, width(pixelWidth)
		, height(pixelHeight)
	{
		//Set some attributes before creating the SDL window
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
		SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, spehs::ApplicationData::MSAA);

		//Create SDL window
		Uint32 sdlFlags = SDL_WINDOW_OPENGL;
		sdlWindow = SDL_CreateWindow("SpehsEngine window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pixelWidth, pixelHeight, sdlFlags);
		if (sdlWindow == nullptr)
		{
			std::string str("Failed to create sdl window!");
			str += SDL_GetError();
			log::error(str);
		}
		
		//Initial settings
		setFullscreen(false);
		setBorderless(true);
	}

	Window::~Window()
	{
		if (glContext)
		{
			SDL_GL_DeleteContext((SDL_GLContext)glContext);
			glContext = nullptr;
		}
		//TODO!!! spehs::graphics::postproc::disablePostProcessing();
		SDL_DestroyWindow(sdlWindow);
	}
				
	void Window::renderBegin()
	{
		graphics::postproc::postProcessingBegin();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void Window::renderEnd()
	{
		graphics::postproc::postProcessingEnd();
		SDL_GL_SwapWindow(sdlWindow);
		checkOpenGLErrors(__FILE__, __LINE__);
	}
		
	void Window::setClearColor(const float _r, const float _g, const float _b, const float _a)
	{
		glClearColor(_r, _g, _b, _a);
		checkOpenGLErrors(__FILE__, __LINE__);
	}

	void Window::setTitle(const char* title)
	{
		SDL_SetWindowTitle(sdlWindow, title);
	}

	void Window::setSize(const int pixelWidth, const int pixelHeight)
	{
		if (pixelWidth != width || pixelHeight != height)
		{
			width = pixelWidth;
			height = pixelHeight;
			SDL_SetWindowSize(sdlWindow, pixelWidth, pixelHeight);
			sizeChangedSignal(width, height);
		}
	}

	void Window::setMinSize(const int minPixelWidth, const int minPixelHeight)
	{
		bool sizeChanged = false;
		if (width < minPixelWidth)
		{
			width = minPixelWidth;
			sizeChanged = true;
		}
		if (height < minPixelHeight)
		{
			height = minPixelHeight;
			sizeChanged = true;
		}
		SDL_SetWindowMinimumSize(sdlWindow, minPixelWidth, minPixelHeight);
		if (sizeChanged)
			sizeChangedSignal(width, height);
	}

	void Window::setMaxSize(const int maxPixelWidth, const int maxPixelHeight)
	{
		bool sizeChanged = false;
		if (width > maxPixelWidth)
		{
			width = maxPixelWidth;
			sizeChanged = true;
		}
		if (height > maxPixelHeight)
		{
			height = maxPixelHeight;
			sizeChanged = true;
		}
		SDL_SetWindowMaximumSize(sdlWindow, maxPixelWidth, maxPixelWidth);
		if (sizeChanged)
			sizeChangedSignal(width, height);
	}

	void Window::setFullscreen(const bool enabled)
	{
		SDL_SetWindowFullscreen(sdlWindow, (SDL_bool)enabled);
	}

	void Window::setBorderless(const bool enabled)
	{
		SDL_SetWindowBordered(sdlWindow, (SDL_bool)enabled);
	}

	int Window::getWidth() const
	{
		return width;
	}

	int Window::getHeight() const
	{
		return height;
	}
}