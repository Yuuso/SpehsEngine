
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
		shaderIndex = DefaultPolygon;
		drawMode = TRIANGLE;
		scaledMatrix = glm::mat4(1.0f);
		scaledRotatedMatrix = glm::mat4(1.0f);
		vertexArray = nullptr;

		lineWidth = 0.0f;
		yaw = 0.0f;
		pitch = 0.0f;
		roll = 0.0f;
		scaleX = 0.0f;
		scaleY = 0.0f;
		scaleZ = 0.0f;
		color = glm::vec4(1.0f);
	}
	Mesh::Mesh(const std::string &_filepath) : Mesh()
	{
		//Check the file type of the given model file
		std::string fileEnding = "";
		auto it = _filepath.end() - 1;
		while (*it != '.')
		{
			fileEnding.push_back(*it);
			it--;
		}

		if (fileEnding == "jbo") //.obj files
			setOBJMesh(_filepath);
		else
			exceptions::fatalError("The models file type is not supported!");
	}
	Mesh::~Mesh()
	{
		if (worldVertexArray != nullptr)
			delete [] worldVertexArray;
		if (vertexArray != nullptr)
			delete [] vertexArray;
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
		modelManager->loadOBJ(_objFilepath, this);
		//WORLD VERTEX ARRAY!!!
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
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.setColor(color);
		}
	}

	void Mesh::setColor(const glm::vec3 &_newColor)
	{
		color = glm::vec4(_newColor, 1.0f);
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.setColor(color);
		}
	}

	void Mesh::setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a)
	{
		color = glm::vec4((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f);
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.setColor(color);
		}
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

	void Mesh::setDrawMode(const DrawMode &_drawMode)
	{
		drawMode = _drawMode;
	}

	void Mesh::setLineWidth(const float &_lineWidth)
	{
		lineWidth = _lineWidth;
	}
}