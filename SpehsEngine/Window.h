#pragma once
#include <string>
#include "ShaderManager.h"


struct SDL_Window;
namespace spehs
{
	class GLSLProgram;
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
		void setPostProcessingShader(std::string vertexShaderPath, std::string fragmentShaderPath);

	private:
		SDL_Window* sdlWindow;
		
		//Post processing effect
		GLSLProgram* glslProgram;
		GLuint fbo, fbo_texture, rbo_depth, vbo_fbo_vertices;
		float fbo_vertices[8];
		GLint vertexPositionLocation;
		GLint textureLocation;
	};
}
extern spehs::Window* mainWindow;