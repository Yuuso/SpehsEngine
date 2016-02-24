#pragma once

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "GLSLProgram.h"


typedef int GLint;
typedef unsigned int GLuint;
namespace SpehsEngine
{
	enum ShaderName : int
	{
		DefaultPolygon = 0,
		DefaultTexture = 1,
		Background = 2,
		Particle = 3,
	};

	struct Shader;
	class BackgroundUniforms;
	class DefaultTextureUniforms;
	class DefaultPolygonUniforms;
	class ParticleUniforms;
	class Uniforms
	{
	public:
		Uniforms(GLSLProgram* _shader);
		virtual ~Uniforms();

		virtual void setUniforms();
		glm::mat4 transformMatrix;
		glm::mat4 cameraMatrix;
		glm::vec4 color;
		GLuint textureDataID; //this is only here because primitives need it, Uniform base class can't send textureData

		virtual BackgroundUniforms* backgroundUniformPtr(){ return nullptr; }
		virtual DefaultTextureUniforms* defaultTextureUniformPtr(){ return nullptr; }
		virtual DefaultPolygonUniforms* defaultPolygonUniformPtr(){ return nullptr; }
		virtual ParticleUniforms* particleUniformPtr(){ return nullptr; }

	protected:
		GLSLProgram* shader;

		GLint transformLocation = 0;
		GLint cameraLocation = 0;
		GLint colorLocation = 0;
	};

#pragma region Individual Uniforms
	class BackgroundUniforms : public Uniforms
	{
	public:
		BackgroundUniforms(GLSLProgram* _shader);
		~BackgroundUniforms();

		void setUniforms();
		float delta;
		glm::vec2 mouse;
		float zoom;
		BackgroundUniforms* backgroundUniformPtr(){ return this; }

	private:
		GLint deltaLocation = 0;
		GLint mouseLocation = 0;
		GLint zoomLocation = 0;
	};
	class DefaultTextureUniforms : public Uniforms
	{
	public:
		DefaultTextureUniforms(GLSLProgram* _shader);
		~DefaultTextureUniforms();

		void setUniforms();
		DefaultTextureUniforms* defaultTextureUniformPtr(){ return this; }

	private:
		GLint textureLocation = 0;
	};
	class DefaultPolygonUniforms : public Uniforms
	{
	public:
		DefaultPolygonUniforms(GLSLProgram* _shader);
		~DefaultPolygonUniforms();

		void setUniforms();
		glm::vec2 mouse;
		DefaultPolygonUniforms* defaultPolygonUniformPtr(){ return this; }

	private:
		GLint mouseLocation = 0;
	};
	class ParticleUniforms : public Uniforms
	{
	public:
		ParticleUniforms(GLSLProgram* _shader);
		~ParticleUniforms();

		void setUniforms();
		float random;
		float zoom;
		ParticleUniforms* particleUniformPtr(){ return this; }

	private:
		GLint randomLocation = 0;
		GLint zoomLocation = 0;
		unsigned int textureLocation = 0;
	};
#pragma endregion

	struct Shader
	{
		Shader(ShaderName _name, GLSLProgram* _shader, Uniforms* _uniforms)
		{
			name = _name;
			shader = _shader;
			uniforms = _uniforms;
		}
		~Shader()
		{
			delete shader;
			delete uniforms;
		}
		ShaderName name;
		GLSLProgram* shader;
		Uniforms* uniforms;
	};

	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		Shader* getShader(ShaderName _shaderName);
		void setUniforms(ShaderName _shaderName);
		void use(ShaderName _shaderName);
		void unuse(ShaderName _shaderName);

	private:
		std::vector<Shader*> shaderPrograms;//TODO:not a vector
	};
}
extern SpehsEngine::ShaderManager* shaderManager;