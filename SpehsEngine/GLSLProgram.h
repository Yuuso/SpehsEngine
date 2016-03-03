#pragma once
#include <string>


typedef int GLint;
typedef unsigned int GLuint;
namespace spehs
{
	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();


		void compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		void compileShadersFromSource(const std::string& vertexShader, const std::string& fragmentShader); //Used for default shaders
		void linkShaders();
		void addAttribute(const std::string& attributeName);

		GLint getUniformLocation(const std::string& uniformName);

		void use();
		void unuse();
		GLint getProgram(){ return programID; }

	private:
		void compileShader(const std::string& filePath, GLuint id);
		void compileShaderFromSource(const std::string& shader, GLuint id);

		int numberOfAttributes;
		GLuint programID;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
	};
}