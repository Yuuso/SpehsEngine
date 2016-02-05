#pragma once
#include <string>


typedef int GLint;
typedef unsigned int GLuint;
namespace SpehsEngine
{
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();


		void compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void linkShaders();
		void addAttribute(const std::string& attributeName);

		GLint getUniformLocation(const std::string& uniformName);

		void use();
		void unuse();
		GLint getProgram(){ return programID; }

	private:
		void compileShader(const std::string& filePath, GLuint id);

		int numberOfAttributes;
		GLuint programID;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
	};
}