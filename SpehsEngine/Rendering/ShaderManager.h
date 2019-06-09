#pragma once

#include "SpehsEngine/Rendering/GLSLProgram.h"

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <string>


typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLuint;


namespace se
{
	namespace rendering
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

			size
		};

		class Uniforms
		{
		public:
			Uniforms(GLSLProgram& _shader);

			virtual void setUniforms();
			glm::mat4 cameraMatrix = glm::mat4(1.0f);

			struct UniformTexture
			{
				UniformTexture(const GLint _location, const bool cubemap = false);
				GLuint textureDataID;
				GLint textureLocation;
				void(*bind)(const GLuint &_textureID, const size_t _index);
			};
			std::vector<UniformTexture> textureData;

		protected:
			GLint cameraLocation = 0;
		};

		class DefaultTextureUniforms : public Uniforms
		{
		public:
			DefaultTextureUniforms(GLSLProgram& _shader);
		};

		class DefaultCubemapUniforms : public Uniforms
		{
		public:
			DefaultCubemapUniforms(GLSLProgram& _shader);
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
		void bind2DTexture(const GLuint& _textureID, const size_t _index);
		void bindCubeMapTexture(const GLuint& _textureID, const size_t _index);
		void setUniform_int(const GLint& _location, const int& _value, const GLsizei count = 1);
		void setUniform_uint(const GLuint& _location, const unsigned int& _value, const GLsizei count = 1);
		void setUniform_float(const GLint& _location, const float& _value, const GLsizei count = 1);
		void setUniform_vec2(const GLint& _location, const glm::vec2 &_value, const GLsizei count = 1);
		void setUniform_vec3(const GLint& _location, const glm::vec3 &_value, const GLsizei count = 1);
		void setUniform_vec4(const GLint& _location, const glm::vec4 &_value, const GLsizei count = 1);
		void setUniform_ivec2(const GLint& _location, const glm::ivec2 &_value, const GLsizei count = 1);
		void setUniform_ivec3(const GLint& _location, const glm::ivec3 &_value, const GLsizei count = 1);
		void setUniform_ivec4(const GLint& _location, const glm::ivec4 &_value, const GLsizei count = 1);
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
			se::log::error("Uniforms type not found!");
			return nullptr;
		}
	}
}