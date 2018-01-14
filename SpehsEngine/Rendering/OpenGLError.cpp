#ifdef _DEBUG
#include "SpehsEngine/Rendering/OpenGLError.h"

#include <iostream>
#include <GL/glew.h>
#include <SDL/SDL.h>

#define STOP_AFTER_GL_ERROR true


namespace spehs
{
	void debugOpenGLError(const std::string &_file, int _line)
	{
		GLenum errorCode(glGetError());

		switch (errorCode)
		{
		case GL_NO_ERROR:
			//Everything is fine.
			break;

		case GL_INVALID_ENUM:
			std::cout << "\nUnacceptable value is specified for an enumeration argument!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case GL_INVALID_VALUE:
			std::cout << "\nA numeric argument is out of range!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case GL_INVALID_OPERATION:
			std::cout << "\nThe specified operation is not allowed in the current state!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case GL_STACK_OVERFLOW:
			std::cout << "\nAn attempt has been made to perform an operation that would cause an internal stack to overflow!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case GL_STACK_UNDERFLOW:
			std::cout << "\nAn attempt has been made to perform an operation that would cause an internal stack to underflow!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case GL_OUT_OF_MEMORY:
			std::cout << "\nThere is not enough memory left to execute the command!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "\nFramebuffer object is not complete!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;

		default:
			//This should never happen!
			std::cout << "\nUndefined OpenGL Error!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
			if (STOP_AFTER_GL_ERROR)
			{
				std::cout << "Press 'Enter' to continue." << std::endl;
				std::cin.ignore();
			}
			break;
		}
	}
}
#endif