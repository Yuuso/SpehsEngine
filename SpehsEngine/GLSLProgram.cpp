#include <fstream>
#include <vector>

#include <GL/glew.h>

#include "GLSLProgram.h"
#include "Console.h"
#include "OpenGLError.h"



namespace SpehsEngine
{
	GLSLProgram::~GLSLProgram()
	{
	}
	GLSLProgram::GLSLProgram() : numberOfAttributes(0), programID(0), vertexShaderID(0), fragmentShaderID(0)
	{
	}


	void GLSLProgram::compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		programID = glCreateProgram();

		vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		if (vertexShaderID == 0)
			console->fatalError("Failed to create vertex shader!");
		fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
		if (fragmentShaderID == 0)
			console->fatalError("Failed to create vertex shader!");

		compileShader(vertexShaderPath, vertexShaderID);
		compileShader(fragmentShaderPath, fragmentShaderID);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}

	
	void GLSLProgram::linkShaders()
	{
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);

		glLinkProgram(programID);
		
		GLint linkStatus = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, (int*) &linkStatus);
		if (linkStatus == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> errorLog(maxLength);
			if (errorLog.size() > 0)
				glGetProgramInfoLog(programID, maxLength, &maxLength, &errorLog[0]);

			glDeleteProgram(programID);

			glDeleteShader(vertexShaderID);
			glDeleteShader(fragmentShaderID);

			if (errorLog.size() > 0)
				std::printf("\n%s", &(errorLog[0]));
			console->fatalError("Shaders failed to link!");
		}
		
		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragmentShaderID);
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}


	void GLSLProgram::addAttribute(const std::string& attributeName)
	{
		glBindAttribLocation(programID, numberOfAttributes++, attributeName.c_str());

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}


	GLint GLSLProgram::getUniformLocation(const std::string& uniformName)
	{
		GLint location = glGetUniformLocation(programID, uniformName.c_str());

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif

		if (location == GL_INVALID_INDEX)
		{
			console->warning("Uniform " + uniformName + " not found in shader!");
			return NULL;
		}

		return location;
	}


	void GLSLProgram::use()
	{
		glUseProgram(programID);
		for (int i = 0; i < numberOfAttributes; i++)
			glEnableVertexAttribArray(i);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}


	void GLSLProgram::unuse()
	{
		glUseProgram(0);
		for (int i = 0; i < numberOfAttributes; i++)
			glDisableVertexAttribArray(i);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
	
	
	void GLSLProgram::compileShader(const std::string& filePath, GLuint id)
	{
		std::ifstream vertexFile(filePath);
		if (vertexFile.fail())
			console->fatalError("Failed to open " + filePath);

		std::string fileContents = "";
		std::string line;

		while (std::getline(vertexFile, line))
			fileContents += line + "\n";
		vertexFile.close();

		const char* contentsPtr = fileContents.c_str();
		glShaderSource(id, 1, &contentsPtr, nullptr);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
		
		glCompileShader(id);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif

		GLint success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);

		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<char> errorLog(maxLength);
			if (errorLog.size() > 0)
				glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

			glDeleteShader(id);

			if (errorLog.size() > 0)
				std::printf("\n%s", &errorLog[0]);
			console->fatalError("Shader " + filePath + " failed to compile!");
		}

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
}