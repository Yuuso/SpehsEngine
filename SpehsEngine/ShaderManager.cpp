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
		spehs::bind2DTexture(textureDataID);
		spehs::setUniform_int(textureLocation, 0);
		Uniforms::setUniforms();
	}

	DefaultMeshUniforms::DefaultMeshUniforms(spehs::GLSLProgram* _shader) : Uniforms(_shader)
	{
		cameraPositionLocation = shader->getUniformLocation("cameraPosition");
	}
	DefaultMeshUniforms::~DefaultMeshUniforms(){}
	void DefaultMeshUniforms::setUniforms()
	{
		spehs::setUniform_vec3(cameraPositionLocation, cameraPosition);
		Uniforms::setUniforms();
	}


	ShaderManager::ShaderManager()
	{
#pragma region Default Shaders
		//
		const std::string defaultPolygonVert =
		{
			"#version 150\n"
			"in vec3 vertexPosition;\n"
			"in vec4 vertexColor;\n"
			"out vec4 fragmentColor;\n"
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
			"in vec4 fragmentColor;\n"
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
			"in vec4 vertexColor;\n"
			"in vec2 vertexUV;\n"
			"out vec4 fragmentColor;\n"
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
			"in vec4 fragmentColor;\n"
			"out vec4 color;\n"
			"uniform sampler2D tex;\n"
			"void main()\n"
			"{\n"
			"	color = texture(tex, texCoord);\n"
			"	color = color * fragmentColor;\n"
			"}\n"
		};
		//
		const std::string defaultMeshVert =
		{
			"#version 150\n"
			"in vec3 vertexPosition;\n"
			"in vec4 vertexColor;\n"
			"in vec3 vertexNormal;\n"
			"out vec3 fragmentPosition;\n"
			"out vec4 fragmentColor;\n"
			"out vec3 fragmentNormal;\n"
			"uniform mat4 cameraMatrix;\n"
			"void main()\n"
			"{\n"
			"	gl_Position = cameraMatrix * vec4(vertexPosition.xyz, 1.0);\n"
			"	fragmentPosition = vertexPosition;\n"
			"	fragmentColor = vertexColor;\n"
			"	fragmentNormal = vertexNormal;\n"
			"}\n"
		};
		const std::string defaultMeshFrag =
		{
			"#version 150\n"
			"in vec3 fragmentPosition;\n"
			"in vec4 fragmentColor;\n"
			"in vec3 fragmentNormal;\n"
			"out vec4 color;\n"
			"uniform vec3 cameraPosition;\n"
			"void main()\n"
			"{\n"
			"	vec3 normal = normalize(fragmentNormal);\n"
			"	vec3 lightPosition = vec3(0.0, 5.0, 0.0);\n"
			"	vec3 lightDirection = normalize(lightPosition - fragmentPosition);\n"
			"	vec3 viewDirection = normalize(cameraPosition - fragmentPosition);\n"
			"	float distance = length(lightPosition - fragmentPosition);\n"
			"	float radius = 20.0;\n"
			"	float attenuation = 1 - pow((distance / radius), 2);\n"
			"	float shininess = 64.0;\n"
			//Ambient
			"	vec3 ambient = 0.1 * fragmentColor.rgb;\n"
			//Diffuse
			"	vec3 diffuse = fragmentColor.rgb;\n"
			//Specular
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
			"	color = vec4(ambient + attenuation * (diffuse + specular), 1.0);\n"
			"}\n"
		};
		//
#pragma endregion

		//DefaultPolygon
		spehs::GLSLProgram* defaultShader = new spehs::GLSLProgram();
		defaultShader->compileShadersFromSource(defaultPolygonVert, defaultPolygonFrag);
		defaultShader->addAttribute("vertexPosition");
		defaultShader->addAttribute("vertexColor");
		defaultShader->linkShaders();
		shaderPrograms.push_back(new spehs::Shader(spehs::DefaultPolygon, defaultShader, new Uniforms(defaultShader)));

		//DefaultTexture
		spehs::GLSLProgram* defaultTexShader = new spehs::GLSLProgram();
		defaultTexShader->compileShadersFromSource(defaultTextureVert, defaultTextureFrag);
		defaultTexShader->addAttribute("vertexPosition");
		defaultTexShader->addAttribute("vertexColor");
		defaultTexShader->addAttribute("vertexUV");
		defaultTexShader->linkShaders();
		shaderPrograms.push_back(new spehs::Shader(spehs::DefaultTexture, defaultTexShader, new DefaultTextureUniforms(defaultTexShader)));

		//DefaultMesh
		spehs::GLSLProgram* defaultMeshShader = new spehs::GLSLProgram();
		defaultMeshShader->compileShadersFromSource(defaultMeshVert, defaultMeshFrag);
		defaultMeshShader->addAttribute("vertexPosition");
		defaultMeshShader->addAttribute("vertexColor");
		defaultMeshShader->addAttribute("vertexNormal");
		defaultTexShader->addAttribute("vertexUV");
		defaultMeshShader->linkShaders();
		shaderPrograms.push_back(new spehs::Shader(spehs::DefaultMesh, defaultMeshShader, new DefaultMeshUniforms(defaultMeshShader)));
		////DefaultMeshTexture
		//spehs::GLSLProgram* defaultMeshTexShader = new spehs::GLSLProgram();
		//defaultTexShader->compileShadersFromSource(defaultTextureVert, defaultTextureFrag);
		//defaultTexShader->addAttribute("vertexPosition");
		//defaultTexShader->addAttribute("textureCoords");
		//defaultTexShader->linkShaders();
		//shaderPrograms.push_back(new spehs::Shader(spehs::DefaultMeshTexture, defaultMeshTexShader, new DefaultTextureUniforms(defaultMeshTexShader)));
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
			spehs::console::error("Trying to access a non-existing shader!");
			return;
		}
		delete shaderPrograms[_index];
		shaderPrograms[_index] = _newShader;
	}
	Shader* ShaderManager::getShader(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::console::error("Trying to access a non-existing shader!");
			return nullptr;
		}
		return shaderPrograms[_index];
	}
	void ShaderManager::setUniforms(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::console::error("Trying to access a non-existing shader!");
			return;
		}
		shaderPrograms[_index]->uniforms->setUniforms();
	}
	void ShaderManager::use(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::console::error("Trying to access a non-existing shader!");
			return;
		}
		shaderPrograms[_index]->shader->use();
	}
	void ShaderManager::unuse(int _index)
	{
		if (shaderPrograms.size() < _index + 1)
		{
			spehs::console::error("Trying to access a non-existing shader!");
			return;
		}
		shaderPrograms[_index]->shader->unuse();
	}


	void bind2DTexture(const GLuint &_textureID)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _textureID);

		checkOpenGLErrors(__FILE__, __LINE__);
	}
	void setUniform_int(const GLint &_location, const int &_value)
	{
		glUniform1i(_location, _value);

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