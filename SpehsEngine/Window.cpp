#include "Exceptions.h"
#include "Window.h"
#include "Exceptions.h"
#include "ApplicationData.h"
#include "OpenGLError.h"
#include "GLSLProgram.h"

#include <SDL/SDL_video.h>
#include <GL/glew.h>


spehs::Window* mainWindow;
namespace spehs
{
	Window::Window() : glslProgram(nullptr)
	{
	}
	Window::~Window()
	{
		deallocatePostProcessingShader();
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
			exceptions::fatalError("\nFailed to create window!");

		//Update window size if it changed
		
		int w, h;
		SDL_GetWindowSize(sdlWindow, &w, &h);
		applicationData->setWindowWidth(w);
		applicationData->setWindowHeight(h);

		SDL_GLContext glContext = SDL_GL_CreateContext(sdlWindow);
		if (glContext == nullptr)
			exceptions::fatalError("\nFailed to create glContext!");

		glewExperimental = GL_TRUE;
		GLenum error = glewInit();
		if (error != GLEW_OK)
			exceptions::fatalError("\nFailed to initialize glew!");

		//Check OpenGL version
		std::printf("*** OpenGL Version: %s ***\n", glGetString(GL_VERSION));

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		
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
		if (applicationData->MSAA > 0)
			glEnable(GL_MULTISAMPLE);
		
		checkOpenGLErrors(__FILE__, __LINE__);

		return 0;
	}


	void Window::disablePostProcessingShader()
	{
		deallocatePostProcessingShader();
	}


	void Window::deallocatePostProcessingShader()
	{
		if (glslProgram)
		{
			delete glslProgram;
			glslProgram = nullptr;
			glDeleteBuffers(1, &vbo_fbo_vertices);
			glDeleteRenderbuffers(1, &rbo_depth);
			glDeleteTextures(1, &fbo_texture);
			glDeleteFramebuffers(1, &fbo);
		}
	}


	void Window::setPostProcessingShader(std::string vertexShaderPath, std::string fragmentShaderPath)
	{
		deallocatePostProcessingShader();

		////Post processing
		glGenTextures(1, &fbo_texture);
		glGenRenderbuffers(1, &rbo_depth);
		glGenFramebuffers(1, &fbo);
		glGenBuffers(1, &vbo_fbo_vertices);
		fbo_vertices[0] = -1;	fbo_vertices[1] = -1;	fbo_vertices[2] = 1;	fbo_vertices[3] = -1;	fbo_vertices[4] = -1;	fbo_vertices[5] = 1;	fbo_vertices[6] = 1;	fbo_vertices[7] = 1;
		
		/* Texture */
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, applicationData->getWindowWidth(), applicationData->getWindowHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		/* Depth buffer */
		glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, applicationData->getWindowWidth(), applicationData->getWindowHeight());
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		/* Framebuffer to link everything together */
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
		GLenum status;
		if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
			spehs::exceptions::fatalError("glCheckFramebufferStatus: error " + std::to_string(status));
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glslProgram = new GLSLProgram();
		glslProgram->compileShaders(vertexShaderPath, fragmentShaderPath);
		glslProgram->addAttribute("vertexPosition");
		glslProgram->linkShaders();
		textureLocation = glslProgram->getUniformLocation("texture");
	}


	void Window::renderBegin()
	{
		if (glslProgram)
			glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		checkOpenGLErrors(__FILE__, __LINE__);
	}


	void Window::renderEnd()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		if (glslProgram)
		{//Post processing effects
			glslProgram->use();
			glBindTexture(GL_TEXTURE_2D, fbo_texture);
			glUniform1i(textureLocation, /*GL_TEXTURE*/0);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
			glVertexAttribPointer(
				vertexPositionLocation,  // attribute
				2,                  // number of elements per vertex, here (x,y)
				GL_FLOAT,           // the type of each element
				GL_FALSE,           // take our values as-is
				0,                  // no extra data between each position
				0                   // offset of first element
				);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glslProgram->unuse();
		}

		SDL_GL_SwapWindow(sdlWindow);

		checkOpenGLErrors(__FILE__, __LINE__);
	}


	void Window::clearColor(float _r, float _g, float _b, float _a)
	{
		glClearColor(_r, _g, _b, _a);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
}