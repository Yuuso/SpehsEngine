
#include "Mesh.h"
#include "ModelManager.h"
#include "BatchManager.h"
#include "Console.h"
#include "ShaderManager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define GLM_FORCE_RADIANS


namespace spehs
{
	Mesh::Mesh()
	{
		textureDataID = 0;
		readyForDelete = false;
		renderState = true;
		needUpdate = true;
		shaderIndex = DefaultMesh;
		drawMode = TRIANGLE;
		scaledMatrix = glm::mat4(1.0f);
		scaledRotatedMatrix = glm::mat4(1.0f);

		lineWidth = 0.0f;
		rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		scaleX = 1.0f;
		scaleY = 1.0f;
		scaleZ = 1.0f;
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
	}


	void Mesh::updateVertices()
	{
		if (needUpdate)
		{
			//T = t * R * S
			scaledMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));
			scaledRotatedMatrix = glm::mat4_cast(glm::quat(rotation)) * scaledMatrix;
			transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z)) * scaledRotatedMatrix;

			for (unsigned i = 0; i < worldVertexArray.size(); i++)
			{
				glm::vec4 vertex = transformMatrix * glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, vertexArray[i].position.z, 1.0f);
				worldVertexArray[i].position.setPosition(vertex.x, vertex.y, vertex.z);
			}
			needUpdate = false;
		}
	}

	void Mesh::destroy()
	{
		readyForDelete = true;
	}

	void Mesh::setOBJMesh(const std::string &_objFilepath)
	{
		vertexArray.clear();
		worldVertexArray.clear();
		elementArray.clear();

		modelManager->loadOBJ(_objFilepath, this);
		worldVertexArray = vertexArray;

		needUpdate = true;
	}

	void Mesh::setPosition(const float &_x, const float &_y, const float &_z)
	{
		position.setPosition(_x, _y, _z);
		needUpdate = true;
	}

	void Mesh::setPosition(const glm::vec3 &_newPosition)
	{
		position.setPosition(_newPosition.x, _newPosition.y, _newPosition.z);
		needUpdate = true;
	}

	void Mesh::setPosition(const Position &_newPosition)
	{
		position = _newPosition;
		needUpdate = true;
	}

	void Mesh::setRotation(const float &_yaw, const float &_pitch, const float &_roll)
	{
		rotation.x = _yaw;
		rotation.y = _pitch;
		rotation.z = _roll;
		needUpdate = true;
	}

	void Mesh::setRotation(const glm::vec3 &_newRotation)
	{
		rotation = _newRotation;
		needUpdate = true;
	}

	void Mesh::setScale(const float &_newScale)
	{
		scaleX = _newScale;
		scaleY = _newScale;
		scaleZ = _newScale;
		needUpdate = true;
	}

	void Mesh::setScale(const float &_newScaleX, const float &_newScaleY, const float &_newScaleZ)
	{
		scaleX = _newScaleX;
		scaleY = _newScaleY;
		scaleZ = _newScaleZ;
		needUpdate = true;
	}

	void Mesh::setScale(const glm::vec3 &_newScale)
	{
		scaleX = _newScale.x;
		scaleY = _newScale.y;
		scaleZ = _newScale.z;
		needUpdate = true;
	}

	void Mesh::setColor(const glm::vec4 &_newColor)
	{
		color = _newColor;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(color);
		}
	}

	void Mesh::setColor(const glm::vec3 &_newColor)
	{
		color = glm::vec4(_newColor, 1.0f);
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(color);
		}
	}

	void Mesh::setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a)
	{
		color = glm::vec4((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f);
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(color);
		}
	}

	void Mesh::setColor(const CommonColor& _color)
	{
		switch (_color)
		{
		case WHITE:
			setColor(255, 255, 255);
			break;
		case BLACK:
			setColor(0, 0, 0);
			break;
		case RED:
			setColor(255, 0, 0);
			break;
		case ORANGE:
			setColor(255, 128, 0);
			break;
		case YELLOW:
			setColor(255, 255, 0);
			break;
		case GREEN:
			setColor(0, 255, 0);
			break;
		case CYAN:
			setColor(0, 255, 255);
			break;
		case BLUE:
			setColor(0, 0, 255);
			break;
		case MAGENTA:
			setColor(255, 0, 255);
			break;
		default:
			console::error("Invalid CommonColor: " + std::to_string(_color));
			break;
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
