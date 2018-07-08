
#pragma once

#include <vector>
#include <string>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Rendering/GLSLProgram.h"


typedef int GLint;
typedef unsigned int GLuint;
namespace spehs
{

	enum ShaderName : int
	{//Default Shaders
		DefaultPolygon = 0,
		DefaultTexture = 1,
		DefaultPostProc = 2,
		DefaultText = 3,
		DefaultMesh = 4,
		DefaultTextureMesh = 5,
		DefaultSkybox = 6,

		DefaultShaderCount
	};

	class Uniforms
	{
	public:
		Uniforms(GLSLProgram* _shader);
		virtual ~Uniforms();

		virtual void setUniforms();
		glm::mat4 cameraMatrix;
		GLuint textureDataID; //this is only here because primitives need it, Uniform base class can't send textureData

	protected:
		GLSLProgram* shader;

		GLint cameraLocation = 0;
	};

	class DefaultTextureUniforms : public spehs::Uniforms
	{
	public:
		DefaultTextureUniforms(spehs::GLSLProgram* _shader);
		~DefaultTextureUniforms();

		void setUniforms();

	private:
		GLint textureLocation = 0;
	};

	class DefaultSkyBoxUniforms : public spehs::Uniforms
	{
	public:
		DefaultSkyBoxUniforms(spehs::GLSLProgram* _shader);
		~DefaultSkyBoxUniforms();

		void setUniforms();

	private:
		GLint textureLocation = 0;
	};


	class Shader
	{
	public:
		Shader(int _index, GLSLProgram* _shader, Uniforms* _uniforms)
		{
			shader = _shader;
			uniforms = _uniforms;
			index = _index;
		}
		~Shader()
		{
			delete shader;
			delete uniforms;
		}
		template<typename type> type* getCustomUniforms();

		int index;
		GLSLProgram* shader;
		Uniforms* uniforms;
	};

	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		void pushShader(Shader* _newShader);
		void reload(int _index, Shader* _newShader);

		Shader* getShader(int _index); //NOTE: Don't store shader pointers since it is possible that they expire. Always use getShader to access shaders.
		void setUniforms(int _index);
		void use(int _index);
		void unuse(int _index);
		size_t size();

	private:
		std::vector<Shader*> shaderPrograms;
	};


	//Wrapping functions for OpenGL
	void bind2DTexture(const GLuint &_textureID, const int _index);
	void bindCubeMapTexture(const GLuint &_textureID, const int _index);
	void setUniform_int(const GLint &_location, const int &_value);
	void setUniform_uint(const GLuint &_location, const int &_value);
	void setUniform_float(const GLint &_location, const float &_value);
	void setUniform_vec2(const GLint &_location, const spehs::vec2 &_value);
	void setUniform_vec3(const GLint &_location, const spehs::vec3 &_value);
	void setUniform_vec4(const GLint &_location, const spehs::vec4 &_value);
	void setUniform_ivec2(const GLint &_location, const spehs::ivec2 &_value);
	void setUniform_ivec3(const GLint &_location, const spehs::ivec3 &_value);
	void setUniform_ivec4(const GLint &_location, const spehs::ivec4 &_value);
	void setUniform_mat3(const GLint &_location, const glm::mat3 &_value, bool _transpose = false);
	void setUniform_mat4(const GLint &_location, const glm::mat4 &_value, bool _transpose = false);


	//TEMPLATES:
	template<typename type>
	type* Shader::getCustomUniforms()
	{
		type* temp = dynamic_cast<type*>(uniforms);
		if (temp != nullptr)
		{
			return temp;
		}
		spehs::exceptions::fatalError("Uniforms type not found!");
		return nullptr;
	}
}