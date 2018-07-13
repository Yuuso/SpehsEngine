
#include "stdafx.h"
#include <fstream>
#include <vector>

#include <GL/glew.h>

#include "SpehsEngine/Rendering/GLSLProgram.h"
#include "SpehsEngine/Rendering/Vertex3D.h"
#include "SpehsEngine/Rendering/OpenGLError.h"
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Vertex.h"



namespace se
{
	namespace rendering
	{
		GLSLProgram::~GLSLProgram()
		{
		}
		GLSLProgram::GLSLProgram() : programID(0), vertexShaderID(0), fragmentShaderID(0)
		{
		}


		bool GLSLProgram::compileShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
		{
			programID = glCreateProgram();

			vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
			if (vertexShaderID == 0)
			{
				exceptions::unexpectedError("Failed to create vertex shader!");
				return false;
			}
			fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
			if (fragmentShaderID == 0)
			{
				exceptions::unexpectedError("Failed to create fragment shader!");
				return false;
			}

			compileShader(vertexShaderPath, vertexShaderID);
			compileShader(fragmentShaderPath, fragmentShaderID);

			checkOpenGLErrors(__FILE__, __LINE__);
			return true;
		}
		bool GLSLProgram::compileShadersFromSource(const std::string& vertexShader, const std::string& fragmentShader)
		{
			programID = glCreateProgram();

			vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
			if (vertexShaderID == 0)
			{
				exceptions::unexpectedError("Failed to create vertex shader!");
				return false;
			}
			fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
			if (fragmentShaderID == 0)
			{
				exceptions::unexpectedError("Failed to create fragment shader!");
				return false;
			}

			compileShaderFromSource(vertexShader, vertexShaderID);
			compileShaderFromSource(fragmentShader, fragmentShaderID);

			checkOpenGLErrors(__FILE__, __LINE__);
			return true;
		}


		void GLSLProgram::linkShaders()
		{
			glAttachShader(programID, vertexShaderID);
			glAttachShader(programID, fragmentShaderID);

			glLinkProgram(programID);

			GLint linkStatus = 0;
			glGetProgramiv(programID, GL_LINK_STATUS, (int*)&linkStatus);
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
				exceptions::fatalError("Shaders failed to link!");
			}

			glDetachShader(programID, vertexShaderID);
			glDetachShader(programID, fragmentShaderID);
			glDeleteShader(vertexShaderID);
			glDeleteShader(fragmentShaderID);

			checkOpenGLErrors(__FILE__, __LINE__);
		}


		void GLSLProgram::addAttribute(const VertexAttributePosition _attrib, const std::string& _attributeName)
		{
			glBindAttribLocation(programID, (GLuint)_attrib, _attributeName.c_str());
			attributes.push_back(_attrib);

			checkOpenGLErrors(__FILE__, __LINE__);
		}


		GLint GLSLProgram::getUniformLocation(const std::string& uniformName)
		{
			GLint location = glGetUniformLocation(programID, uniformName.c_str());

			checkOpenGLErrors(__FILE__, __LINE__);

			if (location == GL_INVALID_INDEX)
			{
				se::exceptions::warning("Uniform " + uniformName + " not found in shader!");
				return NULL;
			}

			return location;
		}


		void GLSLProgram::use()
		{
			glUseProgram(programID);
			for (size_t i = 0; i < attributes.size(); i++)
				glEnableVertexAttribArray((GLuint)attributes[i]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}

		void GLSLProgram::set2D()
		{
			for (size_t i = 0; i < attributes.size(); i++)
			{
				switch (attributes[i])
				{
				case VertexAttributePosition::VERTEX_POSITION:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(se::Vertex),
						reinterpret_cast<void*>(offsetof(se::Vertex, position)));
					break;
				case VertexAttributePosition::VERTEX_COLOR:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(se::Vertex),
						reinterpret_cast<void*>(offsetof(se::Vertex, color)));
					break;
				case VertexAttributePosition::VERTEX_UV:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(se::Vertex),
						reinterpret_cast<void*>(offsetof(se::Vertex, uv)));
					break;
				default:
					break;
				}
			}
		}

		void GLSLProgram::set3D()
		{
			for (size_t i = 0; i < attributes.size(); i++)
			{
				switch (attributes[i])
				{
				case VertexAttributePosition::VERTEX_POSITION:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D),
						reinterpret_cast<void*>(offsetof(Vertex3D, position)));
					break;
				case VertexAttributePosition::VERTEX_COLOR:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D),
						reinterpret_cast<void*>(offsetof(Vertex3D, color)));
					break;
				case VertexAttributePosition::VERTEX_UV:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D),
						reinterpret_cast<void*>(offsetof(Vertex3D, uv)));
					break;
				case VertexAttributePosition::VERTEX_NORMAL:
					glVertexAttribPointer((GLuint)VertexAttributePosition::VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D),
						reinterpret_cast<void*>(offsetof(Vertex3D, normal)));
					break;
				default:
					break;
				}
			}
		}


		void GLSLProgram::unuse()
		{
			glUseProgram(0);
			for (size_t i = 0; i < attributes.size(); i++)
				glDisableVertexAttribArray((GLuint)attributes[i]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}


		void GLSLProgram::compileShader(const std::string& filePath, GLuint id)
		{
			std::ifstream vertexFile(filePath);
			if (vertexFile.fail())
				exceptions::fatalError("Failed to open " + filePath);

			std::string fileContents = "";
			std::string line;

			while (std::getline(vertexFile, line))
				fileContents += line + "\n";
			vertexFile.close();

			const char* contentsPtr = fileContents.c_str();
			glShaderSource(id, 1, &contentsPtr, nullptr);

			glCompileShader(id);

			checkOpenGLErrors(__FILE__, __LINE__);

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
				exceptions::unexpectedError("Shader " + filePath + " failed to compile!");
			}
		}
		void GLSLProgram::compileShaderFromSource(const std::string& shader, GLuint id)
		{
			const char* contentsPtr = shader.c_str();
			glShaderSource(id, 1, &contentsPtr, nullptr);

			glCompileShader(id);

			checkOpenGLErrors(__FILE__, __LINE__);

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
				exceptions::unexpectedError("Default shader failed to compile!");
			}
		}
	}
}