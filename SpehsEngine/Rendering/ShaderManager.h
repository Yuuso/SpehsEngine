
#pragma once

#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Rendering/GLSLProgram.h"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <string>


typedef int GLint;
typedef unsigned int GLuint;


namespace se
{
	enum class ShaderName : int
	{
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
		Uniforms(GLSLProgram& _shader);
		virtual ~Uniforms();

		virtual void setUniforms();
		glm::mat4 cameraMatrix = glm::mat4(1.0f);
		GLuint textureDataID = 0; //this is only here because primitives need it, Uniform base class can't send textureData

	protected:
		GLSLProgram& shader;
		GLint cameraLocation = 0;
	};

	class DefaultTextureUniforms : public se::Uniforms
	{
	public:
		DefaultTextureUniforms(GLSLProgram& _shader);
		~DefaultTextureUniforms();

		void setUniforms();

	private:
		GLint textureLocation = 0;
	};

	class DefaultSkyBoxUniforms : public se::Uniforms
	{
	public:
		DefaultSkyBoxUniforms(GLSLProgram& _shader);
		~DefaultSkyBoxUniforms();

		void setUniforms();

	private:
		GLint textureLocation = 0;
	};


	class Shader
	{
	public:
		Shader(const int _index)
			: index(_index)
		{
		}
		~Shader()
		{
			if (uniforms)
				delete uniforms;
		}
		template<typename type> type* getCustomUniforms();

		const int index;
		GLSLProgram shader;
		Uniforms* uniforms = nullptr;
	};

	class ShaderManager
	{
	public:
		ShaderManager();
		~ShaderManager();

		Shader& addShader();
		Shader& getShader(const size_t _index);
		void setUniforms(const size_t _index);
		void use(const size_t _index);
		void set2D(const size_t _index);
		void set3D(const size_t _index);
		void unuse(const size_t _index);
		size_t size() const;

	private:
		std::vector<Shader*> shaderPrograms;
	};


	//Wrapping functions for OpenGL
	void bind2DTexture(const GLuint& _textureID, const unsigned int _index);
	void bindCubeMapTexture(const GLuint& _textureID, const unsigned int _index);
	void setUniform_int(const GLint& _location, const int _value);
	void setUniform_uint(const GLuint& _location, const int _value);
	void setUniform_float(const GLint& _location, const float _value);
	void setUniform_vec2(const GLint& _location, const se::vec2 &_value);
	void setUniform_vec3(const GLint& _location, const se::vec3 &_value);
	void setUniform_vec4(const GLint& _location, const se::vec4 &_value);
	void setUniform_ivec2(const GLint& _location, const se::ivec2 &_value);
	void setUniform_ivec3(const GLint& _location, const se::ivec3 &_value);
	void setUniform_ivec4(const GLint& _location, const se::ivec4 &_value);
	void setUniform_mat3(const GLint& _location, const glm::mat3 &_value, bool _transpose = false);
	void setUniform_mat4(const GLint& _location, const glm::mat4 &_value, bool _transpose = false);


	//TEMPLATES:
	template<typename type>
	type* Shader::getCustomUniforms()
	{
		type* temp = dynamic_cast<type*>(uniforms);
		if (temp != nullptr)
		{
			return temp;
		}
		se::exceptions::fatalError("Uniforms type not found!");
		return nullptr;
	}
}