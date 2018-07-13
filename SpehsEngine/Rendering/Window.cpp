#include "stdafx.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Rendering/GLContext.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/FontManager.h"
//#include "SpehsEngine/Rendering/PostProcessing.h" TODO!!!

#include <SDL/SDL_video.h>
#include <GL/glew.h>

namespace se
{
	namespace rendering
	{
		extern void postProcessingBegin();
		extern void postProcessingEnd();

		thread_local int threadWindowCount = 0;

		Window::Window(const int pixelWidth, const int pixelHeight)
			: sdlWindow(nullptr)
			, glContext(nullptr)
			, textureManager(nullptr)
			, fontManager(nullptr)
			, width(pixelWidth)
			, height(pixelHeight)
		{
			threadWindowCount++;
			if (threadWindowCount > 1)
			{
				se::log::info("A single thread cannot have two windows active at the same time.");
				return;
			}

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
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

			//Create SDL window
			Uint32 sdlFlags = SDL_WINDOW_OPENGL;
			sdlWindow = SDL_CreateWindow("SpehsEngine window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pixelWidth, pixelHeight, sdlFlags);
			if (sdlWindow == nullptr)
			{
				std::string str("Failed to create sdl window!");
				str += SDL_GetError();
				log::error(str);
			}

			//Create open gl context specific to this window
			glContext = new GLContext(*this);

			if (isValid())
			{
				//Initial settings
				setFullscreen(false);
				setBorderless(true);

				//Initialize managers
				textureManager = new TextureManager(*this);
				fontManager = new FontManager(*this);
			}
		}

		Window::~Window()
		{
			threadWindowCount--;
			if (glContext)
			{
				SDL_GL_DeleteContext((SDL_GLContext)glContext);
				glContext = nullptr;
			}
			//TODO!!! disablePostProcessing();
			SDL_DestroyWindow(sdlWindow);
		}

		void Window::renderBegin()
		{
			postProcessingBegin();
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			checkOpenGLErrors(__FILE__, __LINE__);
		}

		void Window::renderEnd()
		{
			postProcessingEnd();
			SDL_GL_SwapWindow(sdlWindow);
			checkOpenGLErrors(__FILE__, __LINE__);
		}

		void Window::setClearColor(const Color& color)
		{
			glClearColor((float)color.r / 255.0f, (float)color.g / 255.0f, (float)color.b / 255.0f, (float)color.a / 255.0f);
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

		bool Window::isValid() const
		{
			return glContext ? glContext->isValid() : false;
		}

		TextureManager* Window::getTextureManager()
		{
			return textureManager;
		}

		const TextureManager* Window::getTextureManager() const
		{
			return textureManager;
		}

		FontManager* Window::getFontManager()
		{
			return fontManager;
		}

		const FontManager* Window::getFontManager() const
		{
			return fontManager;
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
}