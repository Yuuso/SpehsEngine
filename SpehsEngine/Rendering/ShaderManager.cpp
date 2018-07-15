#include "stdafx.h"

#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/OpenGLError.h"

#include <GL/glew.h>

/*FLOAT*///#define GLSL_COLOR_VEC_TYPE "vec4"
/*UBYTE*/#define GLSL_COLOR_VEC_TYPE "lowp vec4"



namespace se
{
	namespace rendering
	{
		Uniforms::Uniforms(GLSLProgram& _shader)
			: shader(_shader)
		{
			cameraLocation = shader.getUniformLocation("cameraMatrix");
		}
		Uniforms::~Uniforms()
		{}
		void Uniforms::setUniforms()
		{
			setUniform_mat4(cameraLocation, cameraMatrix);
			checkOpenGLErrors(__FILE__, __LINE__);
		}

		DefaultTextureUniforms::DefaultTextureUniforms(GLSLProgram& _shader) : Uniforms(_shader)
		{
			textureLocation = shader.getUniformLocation("tex");
		}
		DefaultTextureUniforms::~DefaultTextureUniforms() {}
		void DefaultTextureUniforms::setUniforms()
		{
			bind2DTexture(textureDataID, 0);
			setUniform_int(textureLocation, 0);
			Uniforms::setUniforms();
		}

		DefaultSkyBoxUniforms::DefaultSkyBoxUniforms(GLSLProgram& _shader) : Uniforms(_shader)
		{
			textureLocation = shader.getUniformLocation("tex");
		}
		DefaultSkyBoxUniforms::~DefaultSkyBoxUniforms() {}
		void DefaultSkyBoxUniforms::setUniforms()
		{
			bindCubeMapTexture(textureDataID, 0);
			setUniform_int(textureLocation, 0);
			Uniforms::setUniforms();
		}


		void buildDefaultShader(Shader& shader)
		{
#pragma region Default Shaders
			//
			const std::string defaultPolygonVert =
			{
				"#version 150\n"
				"in vec2 vertexPosition;\n"
				"in " GLSL_COLOR_VEC_TYPE " vertexColor;\n"
				"out " GLSL_COLOR_VEC_TYPE " fragmentColor;\n"
				"uniform mat4 cameraMatrix;\n"
				"void main()\n"
				"{\n"
				"	gl_Position = cameraMatrix * vec4(vertexPosition.xy, 0.0, 1.0);\n"
				"	fragmentColor = vertexColor;\n"
				"}\n"
			};
			const std::string defaultPolygonFrag =
			{
				"#version 150\n"
				"in " GLSL_COLOR_VEC_TYPE " fragmentColor;\n"
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
				"in vec2 vertexPosition;\n"
				"in " GLSL_COLOR_VEC_TYPE" vertexColor;\n"
				"in vec2 vertexUV;\n"
				"out " GLSL_COLOR_VEC_TYPE " fragmentColor;\n"
				"out vec2 texCoord;\n"
				"uniform mat4 cameraMatrix;\n"
				"void main()\n"
				"{\n"
				"	gl_Position = cameraMatrix * vec4(vertexPosition.xy, 0.0, 1.0);\n"
				"	fragmentColor = vertexColor;\n"
				"	texCoord = vertexUV;\n"
				"}\n"
			};
			const std::string defaultTextureFrag =
			{
				"#version 150\n"
				"in vec2 texCoord;\n"
				"in " GLSL_COLOR_VEC_TYPE " fragmentColor; \n"
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
				"in " GLSL_COLOR_VEC_TYPE " vertexColor;"
				"out " GLSL_COLOR_VEC_TYPE " fragColor;\n"
				"in vec2 vertexUV;\n"
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
				"in " GLSL_COLOR_VEC_TYPE " fragColor;"
				"out vec4 color;\n"
				"uniform sampler2D tex;\n"
				"void main()\n"
				"{\n"
				"	color = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r);\n"
				"	color = color * fragColor;\n"
				"}\n"
			};
			//
			const std::string defaultMeshVert =
			{
				"#version 150\n"
				"in vec3 vertexPosition;\n"
				"in " GLSL_COLOR_VEC_TYPE " vertexColor;\n"
				"in vec3 vertexNormal;\n"
				"out vec3 fragmentPosition;\n"
				"out " GLSL_COLOR_VEC_TYPE " fragmentColor;\n"
				"out vec3 fragmentNormal;\n"
				"uniform mat4 cameraMatrix;\n"
				"void main()\n"
				"{\n"
				"	gl_Position = cameraMatrix * vec4(vertexPosition.xyz, 1.0);\n"
				"	fragmentPosition = vertexPosition;\n"
				"	fragmentColor = vertexColor;\n"
				"	fragmentNormal = normalize(vertexNormal);\n"
				"}\n"
			};
			const std::string defaultMeshFrag =
			{
				"#version 150\n"
				"in vec3 fragmentPosition;\n"
				"in " GLSL_COLOR_VEC_TYPE " fragmentColor;\n"
				"in vec3 fragmentNormal;\n"
				"out vec4 color;\n"
				"void main()\n"
				"{\n"
				"	vec3 normal = (fragmentNormal);\n"
				"	vec3 lightPosition = vec3(0.0, 0.0, 0.0);\n"
				"	vec3 lightDirection = normalize(lightPosition - fragmentPosition);\n"
				"	vec3 viewDirection = normalize(-fragmentPosition);\n"
				"	float distance = length(lightPosition - fragmentPosition);\n"
				"	float radius = 100.0;\n"
				"	float attenuation = 1 - pow((distance / radius), 2);\n"
				"	float shininess = 64.0;\n"
				"	vec3 ambient = 0.25 * fragmentColor.rgb;\n"
				"	vec3 diffuse = fragmentColor.rgb;\n"
				"	vec3 specular = fragmentColor.rgb;\n"
				"	float spec = 0.0;\n"
				"	float lambertian = max(dot(lightDirection, normal), 0.0);\n"
				"	if(lambertian > 0.0)\n"
				"	{\n"
				"		vec3 halfwayDirection = normalize(lightDirection + viewDirection);\n"
				"		float specAngle = max(dot(normal, halfwayDirection), 0.0);\n"
				"		spec = pow(specAngle, shininess);\n"
				"	}\n"
				"	diffuse = diffuse * lambertian;\n"
				"	specular = specular * spec;\n"
				"	color = vec4(ambient + attenuation * (diffuse + specular), fragmentColor.a);\n"
				"}\n"
			};
			//
			const std::string defaultTextureMeshVert =
			{
				"#version 150\n"
				"in vec3 vertexPosition;\n"
				"in " GLSL_COLOR_VEC_TYPE " vertexColor;\n"
				"in vec2 vertexUV;\n"
				"in vec3 vertexNormal;\n"
				"out vec3 fragmentPosition;\n"
				"out vec4 fragmentColor;\n"
				"out vec2 fragmentUV;\n"
				"out vec3 fragmentNormal;\n"
				"uniform mat4 cameraMatrix;\n"
				"void main()\n"
				"{\n"
				"	gl_Position = cameraMatrix * vec4(vertexPosition.xyz, 1.0);\n"
				"	fragmentPosition = vertexPosition;\n"
				"	fragmentColor = vertexColor;\n"
				"	fragmentNormal = vertexNormal;\n"
				"	fragmentUV = vertexUV;\n"
				"}\n"
			};
			const std::string defaultTextureMeshFrag =
			{
				"#version 150\n"
				"in vec3 fragmentPosition;\n"
				"in " GLSL_COLOR_VEC_TYPE " fragmentColor;\n"
				"in vec2 fragmentUV;\n"
				"in vec3 fragmentNormal;\n"
				"out vec4 color;\n"
				"uniform sampler2D tex;\n"
				"void main()\n"
				"{\n"
				"	vec3 normal = (fragmentNormal);\n"
				"	vec3 lightPosition = vec3(0.0, 5.0, 0.0);\n"
				"	vec3 lightDirection = normalize(lightPosition - fragmentPosition);\n"
				"	vec3 viewDirection = normalize(-fragmentPosition);\n"
				"	float distance = length(lightPosition - fragmentPosition);\n"
				"	float radius = 20.0;\n"
				"	float attenuation = 1 - pow((distance / radius), 2);\n"
				"	float shininess = 64.0;\n"
				"	vec3 ambient = texture(tex, fragmentUV).rgb;\n"
				"	vec3 diffuse = fragmentColor.rgb;\n"
				"	vec3 specular = fragmentColor.rgb;\n"
				"	float spec = 0.0;\n"
				"	float lambertian = max(dot(lightDirection, normal), 0.0);\n"
				"	if(lambertian > 0.0)\n"
				"	{\n"
				"		vec3 halfwayDirection = normalize(lightDirection + viewDirection);\n"
				"		float specAngle = max(dot(normal, halfwayDirection), 0.0);\n"
				"		spec = pow(specAngle, shininess);\n"
				"	}\n"
				"	diffuse = diffuse * lambertian;\n"
				"	specular = specular * spec;\n"
				"	color = vec4(ambient + attenuation * (diffuse + specular), fragmentColor.a);\n"
				"}\n"
			};
			//
			const std::string defaultSkyBoxVert =
			{
				"#version 150\n"
				"in vec3 vertexPosition;\n"
				"out vec3 fragmentPosition;\n"
				"uniform mat4 cameraMatrix;\n"
				"void main()\n"
				"{\n"
				"	vec4 pos = mat4(mat3(cameraMatrix)) * vec4(vertexPosition.xyz, 1.0);\n"
				"	gl_Position = pos.xyww;\n"
				"	fragmentPosition = vertexPosition;\n"
				"}\n"
			};
			const std::string defaultSkyBoxFrag =
			{
				"#version 150\n"
				"in vec3 fragmentPosition;\n"
				"out vec4 color;\n"
				"uniform samplerCube tex;\n"
				"void main()\n"
				"{\n"
				"	color = texture(tex, fragmentPosition);\n"
				"}\n"
			};
			//
#pragma endregion
			switch ((ShaderName)shader.index)
			{
			case ShaderName::DefaultPolygon:
				shader.shader.compileShadersFromSource(defaultPolygonVert, defaultPolygonFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
				shader.shader.linkShaders();
				shader.uniforms = new Uniforms(shader.shader);
				break;
			case ShaderName::DefaultTexture:
				shader.shader.compileShadersFromSource(defaultTextureVert, defaultTextureFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_UV, "vertexUV");
				shader.shader.linkShaders();
				shader.uniforms = new DefaultTextureUniforms(shader.shader);
				break;
			case ShaderName::DefaultPostProc:
				shader.shader.compileShadersFromSource(defaultPostProcVert, defaultPostProcFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.linkShaders();
				shader.uniforms = new DefaultTextureUniforms(shader.shader);
				break;
			case ShaderName::DefaultText:
				shader.shader.compileShadersFromSource(defaultTextVert, defaultTextFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_UV, "vertexUV");
				shader.shader.linkShaders();
				shader.uniforms = new DefaultTextureUniforms(shader.shader);
				break;
			case ShaderName::DefaultMesh:
				shader.shader.compileShadersFromSource(defaultMeshVert, defaultMeshFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_NORMAL, "vertexNormal");
				shader.shader.linkShaders();
				shader.uniforms = new Uniforms(shader.shader);
				break;
			case ShaderName::DefaultTextureMesh:
				shader.shader.compileShadersFromSource(defaultTextureMeshVert, defaultTextureMeshFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_COLOR, "vertexColor");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_UV, "vertexUV");
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_NORMAL, "vertexNormal");
				shader.shader.linkShaders();
				shader.uniforms = new DefaultTextureUniforms(shader.shader);
				break;
			case ShaderName::DefaultSkybox:
				shader.shader.compileShadersFromSource(defaultSkyBoxVert, defaultSkyBoxFrag);
				shader.shader.addAttribute(VertexAttributePosition::VERTEX_POSITION, "vertexPosition");
				shader.shader.linkShaders();
				shader.uniforms = new DefaultSkyBoxUniforms(shader.shader);
				break;
			default:
				log::error("Default shader index out of reach!");
				break;
			}
		}
		ShaderManager::ShaderManager()
		{
			for (size_t i = 0; i < (size_t)ShaderName::DefaultShaderCount; i++)
			{
				buildDefaultShader(addShader());
			}
		}
		ShaderManager::~ShaderManager()
		{
			for (size_t i = 0; i < shaderPrograms.size(); i++)
				delete shaderPrograms[i];
		}
		Shader& ShaderManager::addShader()
		{
			const int index = (int)shaderPrograms.size();
			shaderPrograms.push_back(new Shader(index));
			return *shaderPrograms.back();
		}
		Shader& ShaderManager::getShader(const size_t _index)
		{
			if (shaderPrograms.size() < _index + 1)
			{
				log::error("ShaderManager::getShader is trying to access a non-existing shader!");
			}
			return *shaderPrograms[_index];
		}
		void ShaderManager::setUniforms(const size_t _index)
		{
			if (shaderPrograms.size() < _index + 1)
			{
				log::error("ShaderManager::setUniforms is trying to access a non-existing shader!");
			}
			shaderPrograms[_index]->uniforms->setUniforms();
		}
		void ShaderManager::use(const size_t _index)
		{
			if (shaderPrograms.size() < _index + 1)
			{
				log::error("ShaderManager::use is trying to access a non-existing shader!");
			}
			shaderPrograms[_index]->shader.use();
		}
		void ShaderManager::set2D(const size_t _index)
		{
			if (shaderPrograms.size() < _index + 1)
			{
				log::error("ShaderManager::set2D is trying to access a non-existing shader!");
			}
			shaderPrograms[_index]->shader.set2D();
		}
		void ShaderManager::set3D(const size_t _index)
		{
			if (shaderPrograms.size() < _index + 1)
			{
				log::error("ShaderManager::set3D is trying to access a non-existing shader!");
			}
			shaderPrograms[_index]->shader.set3D();
		}
		void ShaderManager::unuse(const size_t _index)
		{
			if (shaderPrograms.size() < _index + 1)
			{
				log::error("ShaderManager::unuse is trying to access a non-existing shader!");
			}
			shaderPrograms[_index]->shader.unuse();
		}
		size_t ShaderManager::size() const
		{
			return shaderPrograms.size();
		}


		void bind2DTexture(const GLuint &_textureID, const unsigned int _index)
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
				log::error("Incorrect texture index! (bind2DTexture)");
				break;
			}
			glBindTexture(GL_TEXTURE_2D, _textureID);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void bindCubeMapTexture(const GLuint& _textureID, const unsigned int _index)
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
				log::error("Incorrect texture index! (bindCubeMapTexture)");
				break;
			}
			glEnable(GL_TEXTURE_CUBE_MAP);
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_int(const GLint& _location, const int _value)
		{
			glUniform1i(_location, _value);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_uint(const GLuint& _location, const int _value)
		{
			glUniform1ui(_location, _value);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_float(const GLint& _location, const float _value)
		{
			glUniform1f(_location, _value);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_vec2(const GLint& _location, const glm::vec2 &_value)
		{
			glUniform2fv(_location, 1, &_value[0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_vec3(const GLint& _location, const glm::vec3 &_value)
		{
			glUniform3fv(_location, 1, &_value[0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_vec4(const GLint& _location, const glm::vec4 &_value)
		{
			glUniform4fv(_location, 1, &_value[0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_ivec2(const GLint& _location, const glm::ivec2 &_value)
		{
			glUniform2iv(_location, 1, &_value[0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_ivec3(const GLint& _location, const glm::ivec3 &_value)
		{
			glUniform3iv(_location, 1, &_value[0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_ivec4(const GLint& _location, const glm::ivec4 &_value)
		{
			glUniform4iv(_location, 1, &_value[0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_mat3(const GLint& _location, const glm::mat3 &_value, const bool _transpose)
		{
			glUniformMatrix3fv(_location, 1, (GLboolean)_transpose, &_value[0][0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
		void setUniform_mat4(const GLint& _location, const glm::mat4 &_value, const bool _transpose)
		{
			glUniformMatrix4fv(_location, 1, (GLboolean)_transpose, &_value[0][0]);

			checkOpenGLErrors(__FILE__, __LINE__);
		}
	}
}