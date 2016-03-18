
#include "Mesh.h"
#include "ModelManager.h"
#include "BatchManager.h"
#include "Console.h"
#include "ShaderManager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_FORCE_RADIANS

namespace spehs
{
	Mesh::Mesh()
	{
		Vertex* worldVertexArray = nullptr;
		GLuint numVertices = 0;
		textureDataID = 0;
		readyForDelete = false;
		renderState = true;
		useColor = false;
		shaderIndex = DefaultPolygon;
		scaledMatrix = glm::mat4(1.0f);
		scaledRotatedMatrix = glm::mat4(1.0f);
		vertexArray = nullptr;

		yaw = 0.0f;
		pitch = 0.0f;
		roll = 0.0f;
		scaleX = 0.0f;
		scaleY = 0.0f;
		scaleZ = 0.0f;
		normalArray = nullptr;
		color = glm::vec4(1.0f);
	}
	Mesh::Mesh(const std::string &_filepath) : Mesh()
	{
		//Check here what file type the given filepath is
		//For now we assume that it's all obj files...
		modelManager->loadOBJ(_filepath, vertexArray, elementArray, normalArray);
	}
	Mesh::~Mesh()
	{
		if (worldVertexArray != nullptr)
			delete [] worldVertexArray;
		if (vertexArray != nullptr)
			delete [] vertexArray;
		if (normalArray)
			delete [] normalArray;
	}


	void Mesh::updateVertices()
	{
		static glm::vec4 vertex;
		scaledMatrix = glm::scale(glm::mat4(), glm::vec3(scaleX, scaleY, 0.0f));
		scaledRotatedMatrix = scaledMatrix * glm::mat4_cast(glm::fquat(glm::vec3(roll, pitch, yaw))); //Make sure these are correct
		for (unsigned int i = 0; i < numVertices; i++)
		{
			vertex = glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, vertexArray[i].position.z, 1.0f);
			vertex = scaledRotatedMatrix * vertex;
			worldVertexArray[i].position.setPosition(vertex.x + position.x, vertex.y + position.y, vertex.z + position.z);
		}
	}

	void Mesh::destroy()
	{
		readyForDelete = true;
	}

	void Mesh::setOBJMesh(const std::string &_objFilepath)
	{
		modelManager->loadOBJ(_objFilepath, vertexArray, elementArray, normalArray);
	}

	void Mesh::setPosition(const float &_x, const float &_y, const float &_z)
	{
		position.setPosition(_x, _y, _z);
	}

	void Mesh::setPosition(const glm::vec3 &_newPosition)
	{
		position.setPosition(_newPosition.x, _newPosition.y, _newPosition.z);
	}

	void Mesh::setPosition(const Position &_newPosition)
	{
		position = _newPosition;
	}

	void Mesh::setRotation(const float &_yaw, const float &_pitch, const float &_roll)
	{
		yaw = _yaw;
		pitch = _pitch;
		roll = _roll;
	}

	void Mesh::setRotation(const glm::vec3 &_newRotation)
	{
		yaw = _newRotation.x;
		pitch = _newRotation.y;
		roll = _newRotation.z;
	}

	void Mesh::setScale(const float &_newScale)
	{
		scaleX = _newScale;
		scaleY = _newScale;
		scaleZ = _newScale;
	}

	void Mesh::setScale(const float &_newScaleX, const float &_newScaleY, const float &_newScaleZ)
	{
		scaleX = _newScaleX;
		scaleY = _newScaleY;
		scaleZ = _newScaleZ;
	}

	void Mesh::setScale(const glm::vec3 &_newScale)
	{
		scaleX = _newScale.x;
		scaleY = _newScale.y;
		scaleZ = _newScale.z;
	}

	void Mesh::setColor(const glm::vec4 &_newColor)
	{
		color = _newColor;
	}

	void Mesh::setColor(const glm::vec3 &_newColor)
	{
		color = glm::vec4(_newColor, 1.0f);
	}

	void Mesh::setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a)
	{
		color = glm::vec4(_r, _g, _b, _a);
	}

	void Mesh::setRenderState(const bool _newState)
	{
		renderState = _newState;
	}

	void Mesh::setShader(const int &_newShaderIndex)
	{
		if (shaderManager->getShader(_newShaderIndex) == nullptr)
		{
			spehs::console::error("Trying to set a non-existing shader to Mesh!");
			return;
		}
		shaderIndex = _newShaderIndex;
	}
}
