#include "SpehsEngine/Rendering/GLContext.h"
#include "SpehsEngine/Rendering/Window.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Core/Log.h"
#include <SDL/SDL_video.h>
#include <GL/glew.h>

namespace se
{
	GLContext::GLContext(Window& _window)
		: window(_window)
		, glContext(nullptr)
		, valid(true)
	{
		//From the SDL wiki: create an OpenGL context for use with an OpenGL window, and make it current.
		glContext = (void*)SDL_GL_CreateContext(window.sdlWindow);
		if (glContext == nullptr)
		{
			std::string str("Failed to create glContext!");
			str += SDL_GetError();
			log::error(str);
			valid = false;
			return;
		}

		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			log::error("Failed to initialize glew!");
			valid = false;
			return;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClearDepth(1.0f);

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
		int MSAA;
		SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &MSAA);
		if (MSAA > 0)
			glEnable(GL_MULTISAMPLE);

		//Check OpenGL version
		const GLubyte* glString = glGetString(GL_VERSION);
		_ASSERT(glString);
		const std::string openGLVersionString = (const char*)glGetString(GL_VERSION);
		log::info("GLContext created. OpenGL version: " + openGLVersionString);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	GLContext::~GLContext()
	{
	}
}

