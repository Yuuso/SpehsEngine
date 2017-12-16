
#pragma once

#include <string>
#include <vector>


typedef int GLint;
typedef unsigned int GLuint;
namespace spehs
{
	enum VertexAttributePosition
	{
		VERTEX_POSITION = 0,
		VERTEX_COLOR = 1,
		VERTEX_UV = 2
	};

	class GLSLProgram
	{
	public:
		GLSLProgram();
		~GLSLProgram();


		bool compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
		bool compileShadersFromSource(const std::string& vertexShader, const std::string& fragmentShader); //Used for default shaders
		void linkShaders();
		void addAttribute(const VertexAttributePosition _attrib, const std::string& _attributeName);

		GLint getUniformLocation(const std::string& uniformName);

		void use();
		void unuse();
		GLint getProgram(){ return programID; }

	private:
		void compileShader(const std::string& filePath, GLuint id);
		void compileShaderFromSource(const std::string& shader, GLuint id);

		std::vector<VertexAttributePosition> attributes;
		GLuint programID;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
	};
}