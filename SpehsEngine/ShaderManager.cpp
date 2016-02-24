#include "ShaderManager.h"
#include "TextureManager.h"
#include "OpenGLError.h"

#include <GL/glew.h>



SpehsEngine::ShaderManager* shaderManager;
namespace SpehsEngine
{
	Uniforms::Uniforms(GLSLProgram* _shader)
	{
		textureDataID = 0;
		shader = _shader;

		transformLocation = shader->getUniformLocation("transformMatrix");
		cameraLocation = shader->getUniformLocation("cameraMatrix");
		colorLocation = shader->getUniformLocation("polygonColor");
	}
	Uniforms::~Uniforms()
	{}
	void Uniforms::setUniforms()
	{
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &transformMatrix[0][0]);
		glUniform4fv(colorLocation, 1, &color[0]);
		glUniformMatrix4fv(cameraLocation, 1, GL_FALSE, &cameraMatrix[0][0]);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}

	DefaultPolygonUniforms::DefaultPolygonUniforms(SpehsEngine::GLSLProgram* _shader) : Uniforms(_shader)
	{
		mouseLocation = shader->getUniformLocation("mouse");
	}
	DefaultPolygonUniforms::~DefaultPolygonUniforms(){}
	void DefaultPolygonUniforms::setUniforms()
	{
		SpehsEngine::setUniform_vec2(mouseLocation, mouse);
		Uniforms::setUniforms();
	}

	DefaultTextureUniforms::DefaultTextureUniforms(SpehsEngine::GLSLProgram* _shader) : Uniforms(_shader)
	{
		textureLocation = shader->getUniformLocation("tex");
	}
	DefaultTextureUniforms::~DefaultTextureUniforms(){}
	void DefaultTextureUniforms::setUniforms()
	{
		SpehsEngine::bind2DTexture(textureDataID);
		SpehsEngine::setUniform_int(textureLocation, 0);
		Uniforms::setUniforms();
	}


	ShaderManager::ShaderManager()
	{
		//DefaultPolygon
		SpehsEngine::GLSLProgram* defaultShader = new SpehsEngine::GLSLProgram();
		defaultShader->compileShaders("Shaders/polygonShader.vert", "Shaders/polygonShader.frag");
		defaultShader->addAttribute("vertexPosition");
		defaultShader->linkShaders();
		shaderPrograms.push_back(new SpehsEngine::Shader(SpehsEngine::DefaultPolygon, defaultShader, new DefaultPolygonUniforms(defaultShader)));

		//DefaultTexture
		SpehsEngine::GLSLProgram* defaultTexShader = new SpehsEngine::GLSLProgram();
		defaultTexShader->compileShaders("Shaders/defaultTextureShader.vert", "Shaders/defaultTextureShader.frag");
		defaultTexShader->addAttribute("vertexPosition");
		defaultTexShader->linkShaders();
		shaderPrograms.push_back(new SpehsEngine::Shader(SpehsEngine::DefaultTexture, defaultTexShader, new DefaultTextureUniforms(defaultTexShader)));
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
	void ShaderManager::reload(int _shaderIndex, Shader* _newShader)
	{
		delete shaderPrograms[_shaderIndex];
		shaderPrograms[_shaderIndex] = _newShader;
	}
	Shader* ShaderManager::getShader(int _index)
	{
		return shaderPrograms[_index];
	}
	void ShaderManager::setUniforms(int _index)
	{
		shaderPrograms[_index]->uniforms->setUniforms();
	}
	void ShaderManager::use(int _index)
	{
		shaderPrograms[_index]->shader->use();
	}
	void ShaderManager::unuse(int _index)
	{
		shaderPrograms[_index]->shader->unuse();
	}


	void bind2DTexture(const GLuint &_textureID)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textureID);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
	void setUniform_int(const GLint &_location, const int &_value)
	{
		glUniform1i(_location, _value);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
	void setUniform_float(const GLint &_location, const float &_value)
	{
		glUniform1f(_location, _value);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
	void setUniform_vec2(const GLint &_location, const glm::vec2 &_value)
	{
		glUniform2fv(_location, 1, &_value[0]);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
}