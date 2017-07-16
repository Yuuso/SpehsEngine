
#include "ShaderManager.h"
#include "TextureManager.h"
#include "OpenGLError.h"
#include "Console.h"

#include <GL/glew.h>



spehs::ShaderManager* shaderManager;
namespace spehs
{
	Uniforms::Uniforms(GLSLProgram* _shader)
	{
		textureDataID = 0;
		shader = _shader;

		cameraLocation = shader->getUniformLocation("cameraMatrix");
	}
	Uniforms::~Uniforms()
	{}
	void Uniforms::setUniforms()
	{
		spehs::setUniform_mat4(cameraLocation, cameraMatrix);

		checkOpenGLErrors(__FILE__, __LINE__);
	}

	DefaultTextureUniforms::DefaultTextureUniforms(spehs::GLSLProgram* _shader) : Uniforms(_shader)
	{
		textureLocation = shader->getUniformLocation("tex");
	}
	DefaultTextureUniforms::~DefaultTextureUniforms(){}
	void DefaultTextureUniforms::setUniforms()
	{
		spehs::bind2DTexture(textureDataID, 0);
		spehs::setUniform_int(textureLocation, 0);
		Uniforms::setUniforms();
	}


	Shader* buildDefaultShader(const int _index)
	{
#pragma region Default Shaders
		//
		const std::string defaultPolygonVert =
		{
			"#version 150\n"
			"in vec3 vertexPosition;\n"
			"in lowp vec4 vertexColor;\n"
			"out lowp vec4 fragmentColor;\n"
			"uniform mat4 cameraMatrix;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = cameraMatrix * vec4(vertexPosition.xyz, 1.0);\n"
			"	fragmentColor = vertexColor;\n"
			"}\n"
		};
		const std::string defaultPolygonFrag =
		{
			"#version 150\n"
			"in lowp vec4 fragmentColor;\n"
			"out vec4 color;\n"
			"void main()\n"
			"{\n"
			"	color = fragmentColor;\n"
			"}\n"
		};
		//
		const std::string defaultTextureVert =
		{
			"#version 150\n"
			"in vec3 vertexPosition;\n"
			"in lowp vec4 vertexColor;\n"
			"in vec2 vertexUV;\n"
			"out lowp vec4 fragmentColor;\n"
			"out vec2 texCoord;\n"
			"uniform mat4 cameraMatrix;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = cameraMatrix * vec4(vertexPosition.xyz, 1.0);\n"
			"	fragmentColor = vertexColor;\n"
			"	texCoord = vertexUV;\n"
			"}\n"
		};
		const std::string defaultTextureFrag =
		{
			"#version 150\n"
			"in vec2 texCoord;\n"
			"in lowp vec4 fragmentColor;\n"
			"out vec4 color;\n"
			"uniform sampler2D tex;\n"
			"void main()\n"
			"{\n"
			"	color = texture(tex, texCoord);\n"
			"	color = color * fragmentColor;\n"
			"}\n"
		};
		//
		const std::string defaultPostProcVert =
		{
			"#version 150\n"
			"in vec2 vertexPosition;\n"
			"out vec2 texCoord;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = vec4(vertexPosition, 0.0, 1.0);\n"
			"	texCoord = (vertexPosition + 1.0) / 2.0;\n"
			"}\n"
		};
		const std::string defaultPostProcFrag =
		{
			"#version 150\n"
			"in vec2 texCoord;\n"
			"out vec4 outColor;\n"
			"uniform sampler2D tex;\n"
			"void main()\n"
			"{\n"
			"	outColor = texture(tex, texCoord);\n"
			"}\n"
		};
		//
		const std::string defaultTextVert =
		{
			"#version 150\n"
			"in vec2 vertexPosition;\n"
			"in lowp vec4 vertexColor;"
			"in vec2 vertexUV;\n"
			"out lowp vec4 fragColor;\n"
			"out vec2 texCoord;\n"
			"uniform mat4 cameraMatrix;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = cameraMatrix * vec4(vertexPosition.xy, 0.0f, 1.0);\n"
			"	fragColor = vertexColor;\n"
			"	texCoord = vertexUV;\n"
			"}\n"
		};
		const std::string defaultTextFrag =
		{
			"#version 150\n"
			"in vec2 texCoord;\n"
			"in lowp vec4 fragColor;"
			"out vec4 color;\n"
			"uniform sampler2D tex;\n"
			"void main()\n"
			"{\n"
			"	color = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r);\n"
			"	color = color * fragColor;\n"
			"}\n"
		};
#pragma endregion
		Shader* result = nullptr;
		spehs::GLSLProgram* defaultShader = new spehs::GLSLProgram();
		switch (_index)
		{
		case DefaultPolygon:
			defaultShader->compileShadersFromSource(defaultPolygonVert, defaultPolygonFrag);
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
			defaultShader->linkShaders();
			result = new spehs::Shader(spehs::DefaultPolygon, defaultShader, new Uniforms(defaultShader));
			break;
		case DefaultTexture:
			defaultShader->compileShadersFromSource(defaultTextureVert, defaultTextureFrag);
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_UV, "vertexUV");
			defaultShader->linkShaders();
			result = new spehs::Shader(spehs::DefaultTexture, defaultShader, new DefaultTextureUniforms(defaultShader));
			break;
		case DefaultPostProc:
			defaultShader->compileShadersFromSource(defaultPostProcVert, defaultPostProcFrag);
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
			defaultShader->linkShaders();
			result = new spehs::Shader(spehs::DefaultPostProc, defaultShader, new DefaultTextureUniforms(defaultShader));
			break;
		case DefaultText:
			defaultShader->compileShadersFromSource(defaultTextVert, defaultTextFrag);
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
			defaultShader->addAttribute(VertexAttributePosition::VERTEX_UV, "vertexUV");
			defaultShader->linkShaders();
			result = new spehs::Shader(spehs::DefaultText, defaultShader, new DefaultTextureUniforms(defaultShader));
			break;
		default:
			exceptions::fatalError("Default shader index out of reach!");
			break;
		}
		return result;
	}
	ShaderManager::ShaderManager()
	{
		//DefaultPolygon
		shaderPrograms.push_back(buildDefaultShader(DefaultPolygon));

		//DefaultTexture
		shaderPrograms.push_back(buildDefaultShader(DefaultTexture));

		//DefaultPostProc
		shaderPrograms.push_back(buildDefaultShader(DefaultPostProc));

		//DefaultText
		shaderPrograms.push_back(buildDefaultShader(DefaultText));
	}
	ShaderManager::~ShaderManager()
	{
		for (unsigned int i = 0; i < shaderPrograms.size(); i++)
		{
			delete shaderPrograms[i];
		}
	}
	void ShaderManager::pushShader(Shader* _newShader)
	{
		shaderPrograms.push_back(_newShader);
	}
	void ShaderManager::reload(int _index, Shader* _newShader)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::exceptions::unexpectedError("Trying to access a non-existing shader!");
			return;
		}
		delete shaderPrograms[_index];
		shaderPrograms[_index] = _newShader;
	}
	Shader* ShaderManager::getShader(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::exceptions::unexpectedError("Trying to access a non-existing shader!");
			return nullptr;
		}
		return shaderPrograms[_index];
	}
	void ShaderManager::setUniforms(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::exceptions::unexpectedError("Trying to access a non-existing shader!");
			return;
		}
		shaderPrograms[_index]->uniforms->setUniforms();
	}
	void ShaderManager::use(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::exceptions::unexpectedError("Trying to access a non-existing shader!");
			return;
		}
		shaderPrograms[_index]->shader->use();
	}
	void ShaderManager::unuse(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::exceptions::unexpectedError("Trying to access a non-existing shader!");
			return;
		}
		shaderPrograms[_index]->shader->unuse();
	}
	size_t ShaderManager::size()
	{
		return shaderPrograms.size();
	}


	void bind2DTexture(const GLuint &_textureID, const int _index)
	{
		switch (_index)
		{
		case 0:
			glActiveTexture(GL_TEXTURE0);
			break;
		case 1:
			glActiveTexture(GL_TEXTURE1);
			break;
		case 2:
			glActiveTexture(GL_TEXTURE2);
			break;
		case 3:
			glActiveTexture(GL_TEXTURE3);
			break;
		case 4:
			glActiveTexture(GL_TEXTURE4);
			break;
		case 5:
			glActiveTexture(GL_TEXTURE5);
			break;
		default:
			exceptions::fatalError("Incorrect texture index! (bind2DTexture)");
			break;
		}
		glBindTexture(GL_TEXTURE_2D, _textureID);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void bindCubeMapTexture(const GLuint &_textureID, const int _index)
	{
		switch (_index)
		{
		case 0:
			glActiveTexture(GL_TEXTURE0);
			break;
		case 1:
			glActiveTexture(GL_TEXTURE1);
			break;
		case 2:
			glActiveTexture(GL_TEXTURE2);
			break;
		case 3:
			glActiveTexture(GL_TEXTURE3);
			break;
		case 4:
			glActiveTexture(GL_TEXTURE4);
			break;
		case 5:
			glActiveTexture(GL_TEXTURE5);
			break;
		default:
			exceptions::fatalError("Incorrect texture index! (bindCubeMapTexture)");
			break;
		}
		glEnable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_int(const GLint &_location, const int &_value)
	{
		glUniform1i(_location, _value);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_uint(const GLuint &_location, const int &_value)
	{
		glUniform1ui(_location, _value);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_float(const GLint &_location, const float &_value)
	{
		glUniform1f(_location, _value);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_vec2(const GLint &_location, const glm::vec2 &_value)
	{
		glUniform2fv(_location, 1, &_value[0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_vec3(const GLint &_location, const glm::vec3 &_value)
	{
		glUniform3fv(_location, 1, &_value[0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_vec4(const GLint &_location, const glm::vec4 &_value)
	{
		glUniform4fv(_location, 1, &_value[0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_ivec2(const GLint &_location, const glm::ivec2 &_value)
	{
		glUniform2iv(_location, 1, &_value[0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_ivec3(const GLint &_location, const glm::ivec3 &_value)
	{
		glUniform3iv(_location, 1, &_value[0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_ivec4(const GLint &_location, const glm::ivec4 &_value)
	{
		glUniform4iv(_location, 1, &_value[0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_mat3(const GLint &_location, const glm::mat3 &_value, bool _transpose)
	{
		glUniformMatrix3fv(_location, 1, (GLboolean) _transpose, &_value[0][0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_mat4(const GLint &_location, const glm::mat4 &_value, bool _transpose)
	{
		glUniformMatrix4fv(_location, 1, (GLboolean) _transpose, &_value[0][0]);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
}