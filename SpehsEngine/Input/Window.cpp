
#include "SpehsEngine/Input/Window.h"
#include "SpehsEngine/Core/ApplicationData.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Input/OpenGLError.h"
//#include "SpehsEngine/Rendering/PostProcessing.h" TODO!!!

#include <SDL/SDL_video.h>
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

	namespace input
	{
		Window::Window(const int pixelWidth, const int pixelHeight)
			: sdlWindow(nullptr)
		{
			Uint32 sdlFlags = SDL_WINDOW_OPENGL;
			sdlWindow = SDL_CreateWindow("SpehsEngine window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, pixelWidth, pixelHeight, sdlFlags);
			if (sdlWindow == nullptr)
				exceptions::fatalError("\nFailed to create sdl window!");
			
			SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
			if (glContext == nullptr)
				exceptions::fatalError("\nFailed to create glContext!");

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			if (error != GLEW_OK)
				exceptions::fatalError("\nFailed to initialize glew!");
			
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			//Set VSync
			SDL_GL_SetSwapInterval(0);//0 disable, 1 enable, -1 ???

			//Blending
			//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			//Depth
			glDepthFunc(GL_LESS);

			//Face culling
			glFrontFace(GL_CCW);
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);

			//MSAA Antialiasing
			if (spehs::ApplicationData::MSAA > 0)
				glEnable(GL_MULTISAMPLE);

			//Initial settings
			setFullscreen(false);
			setBorderless(true);

			//Check OpenGL version
			const GLubyte* glString = glGetString(GL_VERSION);
			_ASSERT(glString);
			const std::string openGLVersionString = (const char*)glGetString(GL_VERSION);
			log::info("Window created. OpenGL Version: " + openGLVersionString);

			checkOpenGLErrors(__FILE__, __LINE__);
		}

		Window::~Window()
		{
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
			SDL_SetWindowSize(sdlWindow, pixelWidth, pixelHeight);
		}

		void Window::setMinSize(const int minPixelWidth, const int minPixelHeight)
		{
			SDL_SetWindowMinimumSize(sdlWindow, minPixelWidth, minPixelHeight);
		}

		void Window::setMaxSize(const int maxPixelWidth, const int maxPixelHeight)
		{
			SDL_SetWindowMaximumSize(sdlWindow, maxPixelWidth, maxPixelWidth);
		}

		void Window::setFullscreen(const bool enabled)
		{
			SDL_SetWindowFullscreen(sdlWindow, (SDL_bool)enabled);
		}

		void Window::setBorderless(const bool enabled)
		{
			SDL_SetWindowBordered(sdlWindow, (SDL_bool)enabled);
		}
	}
}