#include "OpenGLError.h"

#include <iostream>
#include <GL/glew.h>
#include <SDL/SDL.h>

#define STOP_AFTER_ERROR true


namespace SpehsEngine
{
	void checkOpenGLErrors(const std::string &_file, int _line)
	{
		static GLenum errorCode;
		errorCode = glGetError();

		switch (errorCode)
		{
		case GL_NO_ERROR:
			//Everything is fine.
			break;

		case GL_INVALID_ENUM:
			std::cout << "Unacceptable value is specified for an enumeration argument!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		case GL_INVALID_VALUE:
			std::cout << "A numeric argument is out of range!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		case GL_INVALID_OPERATION:
			std::cout << "The specified operation is not allowed in the current state!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		case GL_STACK_OVERFLOW:
			std::cout << "An attempt has been made to perform an operation that would cause an internal stack to overflow!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		case GL_STACK_UNDERFLOW:
			std::cout << "An attempt has been made to perform an operation that would cause an internal stack to underflow!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		case GL_OUT_OF_MEMORY:
			std::cout << "There is not enough memory left to execute the command!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "Framebuffer object is not complete!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;

		default:
			//This should never happen!
			std::cout << "Undefined OpenGL Error!" << "\n\tfile: " << _file << "\n\tline: " << _line << std::endl;
#ifdef STOP_AFTER_ERROR
			std::cout << "Press 'Enter' to continue." << std::endl;
			std::cin.ignore();
#endif
			break;
		}
	}
}