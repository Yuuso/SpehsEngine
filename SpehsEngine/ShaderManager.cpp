#include "ShaderManager.h"
#include "TextureManager.h"
#include "OpenGLError.h"

#include <GL/glew.h>



SpehsEngine::ShaderManager* shaderManager;
namespace SpehsEngine
{
	//UNIFORMS:
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

	BackgroundUniforms::BackgroundUniforms(GLSLProgram* _shader) : Uniforms(_shader)
	{
		deltaLocation = shader->getUniformLocation("delta");
		mouseLocation = shader->getUniformLocation("mouse");
		zoomLocation = shader->getUniformLocation("zoom");
	}
	BackgroundUniforms::~BackgroundUniforms()
	{}
	void BackgroundUniforms::setUniforms()
	{
		Uniforms::setUniforms();
		glUniform1f(deltaLocation, delta);
		glUniform1f(zoomLocation, zoom);
		glUniform2fv(mouseLocation, 1, &mouse[0]);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}

	DefaultTextureUniforms::DefaultTextureUniforms(GLSLProgram* _shader) : Uniforms(_shader)
	{
		textureLocation = shader->getUniformLocation("tex");
	}
	DefaultTextureUniforms::~DefaultTextureUniforms()
	{}
	void DefaultTextureUniforms::setUniforms()
	{
		Uniforms::setUniforms();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureDataID);
		glUniform1i(textureLocation, 0);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}

	DefaultPolygonUniforms::DefaultPolygonUniforms(GLSLProgram* _shader) : Uniforms(_shader)
	{
		mouseLocation = shader->getUniformLocation("mouse");
	}
	DefaultPolygonUniforms::~DefaultPolygonUniforms()
	{}
	void DefaultPolygonUniforms::setUniforms()
	{
		Uniforms::setUniforms();
		glUniform2fv(mouseLocation, 1, &mouse[0]);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}

	ParticleUniforms::ParticleUniforms(GLSLProgram* _shader) : Uniforms(_shader)
	{
		randomLocation = shader->getUniformLocation("random");
		zoomLocation = shader->getUniformLocation("zoom");
		textureLocation = shader->getUniformLocation("tex");
	}
	ParticleUniforms::~ParticleUniforms()
	{}
	void ParticleUniforms::setUniforms()
	{
		Uniforms::setUniforms();
		glUniform1f(randomLocation, random);
		glUniform1f(zoomLocation, zoom);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureDataID);
		glUniform1i(textureLocation, 0);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}


	//SHADERMANAGER:
	ShaderManager::ShaderManager()
	{
		//DefaultPolygon
		GLSLProgram* defaultShader = new GLSLProgram();
		defaultShader->compileShaders("Shaders/polygonShader.vert", "Shaders/polygonShader.frag");
		defaultShader->addAttribute("vertexPosition");
		defaultShader->linkShaders();
		shaderPrograms.push_back(new Shader(DefaultPolygon, defaultShader, new DefaultPolygonUniforms(defaultShader)));

		//DefaultTexture
		GLSLProgram* defaultTexShader = new GLSLProgram();
		defaultTexShader->compileShaders("Shaders/defaultTextureShader.vert", "Shaders/defaultTextureShader.frag");
		defaultTexShader->addAttribute("vertexPosition");
		defaultTexShader->linkShaders();
		shaderPrograms.push_back(new Shader(DefaultPolygon, defaultTexShader, new DefaultTextureUniforms(defaultTexShader)));

		//Background
		GLSLProgram* backgroundShader = new GLSLProgram();
		backgroundShader->compileShaders("Shaders/backgroundShader.vert", "Shaders/backgroundShader.frag");
		backgroundShader->addAttribute("vertexPosition");
		backgroundShader->linkShaders();
		shaderPrograms.push_back(new Shader(DefaultPolygon, backgroundShader, new BackgroundUniforms(backgroundShader)));

		//Particle
		GLSLProgram* particleShader = new GLSLProgram();
		particleShader->compileShaders("Shaders/particleShader.vert", "Shaders/particleShader.frag");
		particleShader->addAttribute("vertexPosition");
		particleShader->linkShaders();
		shaderPrograms.push_back(new Shader(DefaultPolygon, particleShader, new ParticleUniforms(particleShader)));
	}
	ShaderManager::~ShaderManager()
	{
		for (unsigned int i = 0; i < shaderPrograms.size(); i++)
		{
			delete shaderPrograms[i];
		}
	}

	Shader* ShaderManager::getShader(ShaderName _shaderName)
	{
		return shaderPrograms[(int) _shaderName];
	}
	void ShaderManager::setUniforms(ShaderName _shaderName)
	{
		shaderPrograms[(int) _shaderName]->uniforms->setUniforms();
	}
	void ShaderManager::use(ShaderName _shaderName)
	{
		shaderPrograms[(int) _shaderName]->shader->use();
	}
	void ShaderManager::unuse(ShaderName _shaderName)
	{
		shaderPrograms[(int) _shaderName]->shader->unuse();
	}
}