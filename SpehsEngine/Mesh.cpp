
#include "Mesh.h"
#include "ModelManager.h"
#include "BatchManager.h"
#include "Console.h"
#include "ShaderManager.h"
#include "TextureManager.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_RADIANS


int64_t meshAllocations;
int64_t meshDeallocations;


namespace spehs
{
	Mesh::Mesh()
	{
#ifdef _DEBUG
		meshAllocations++;
#endif

		textureDataID = 0;
		readyForDelete = false;
		renderState = true;
		needUpdate = true;
		blending = false;
		shaderIndex = DefaultMesh;
		drawMode = TRIANGLE;
		scaledMatrix = glm::mat4(1.0f);
		scaledRotatedMatrix = glm::mat4(1.0f);
		transformMatrix = glm::mat4(1.0f);
		normalMatrix = glm::mat4(1.0f);

		lineWidth = 0.0f;
		rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		scaleX = 1.0f;
		scaleY = 1.0f;
		scaleZ = 1.0f;
		color = glm::vec4(1.0f);
	}
	Mesh::Mesh(const std::string &_filepath) : Mesh()
	{
		setMesh(_filepath);
	}
	Mesh::~Mesh()
	{
#ifdef _DEBUG
		meshDeallocations++;
#endif
	}


	void Mesh::updateVertices()
	{
		if (needUpdate)
		{
			//T = t * R * S
			scaledMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));
			scaledRotatedMatrix = glm::mat4_cast(glm::quat(rotation)) * scaledMatrix;
			transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, position.z)) * scaledRotatedMatrix;
			normalMatrix = glm::mat4(glm::inverse(glm::transpose(glm::mat3(transformMatrix))));

			for (unsigned i = 0; i < worldVertexArray.size(); i++)
			{
				//Vertices
				glm::vec4 vertex = transformMatrix * glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, vertexArray[i].position.z, 1.0f);
				worldVertexArray[i].position.setPosition(vertex.x, vertex.y, vertex.z);
				//Normals				
				vertex = normalMatrix * glm::vec4(vertexArray[i].normal.x, vertexArray[i].normal.y, vertexArray[i].normal.z, 1.0f);
				worldVertexArray[i].normal.setPosition(vertex.x, vertex.y, vertex.z);
			}
			needUpdate = false;
		}
	}

	void Mesh::destroy()
	{
		readyForDelete = true;
	}

	void Mesh::setMesh(const std::string &_filepath)
	{
		vertexArray.clear();
		worldVertexArray.clear();
		elementArray.clear();

		if (_filepath == "InfinitePlane")
		{
			//WIP
			vertexArray.push_back(Vertex3D(Position(0.0f, 0.0f, 1.0f))); //Need to set z>w in shader
			vertexArray.push_back(Vertex3D(Position(1.0f, 0.0f, 0.0f)));
			vertexArray.push_back(Vertex3D(Position(0.0f, 1.0f, 0.0f)));
			vertexArray.push_back(Vertex3D(Position(-1.0f, 0.0f, 0.0f)));
			vertexArray.push_back(Vertex3D(Position(0.0f, -1.0f, 0.0f)));

			elementArray.push_back(0);
			elementArray.push_back(1);
			elementArray.push_back(2);
			elementArray.push_back(0);
			elementArray.push_back(2);
			elementArray.push_back(3);
			elementArray.push_back(0);
			elementArray.push_back(3);
			elementArray.push_back(4);
			elementArray.push_back(0);
			elementArray.push_back(4);
			elementArray.push_back(1);
		}
		else
		{
			modelManager->loadModel(_filepath, this);
			worldVertexArray = vertexArray;
		}

		needUpdate = true;
	}

	void Mesh::setPosition(const float &_x, const float &_y, const float &_z)
	{
#ifdef _DEBUG
		if (_x != _x || _y != _y || _z != _z)
		{
			console::error("Position values corrupted!");
		}
#endif
		position.setPosition(_x, _y, _z);
		needUpdate = true;
	}

	void Mesh::setPosition(const glm::vec3 &_newPosition)
	{
#ifdef _DEBUG
		if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y || _newPosition.z != _newPosition.z)
		{
			console::error("Position values corrupted!");
		}
#endif
		position.setPosition(_newPosition.x, _newPosition.y, _newPosition.z);
		needUpdate = true;
	}

	void Mesh::setPosition(const Position &_newPosition)
	{
#ifdef _DEBUG
		if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y || _newPosition.z != _newPosition.z)
		{
			console::error("Position values corrupted!");
		}
#endif
		position = _newPosition;
		needUpdate = true;
	}

	void Mesh::setRotation(const float &_yaw, const float &_pitch, const float &_roll)
	{
#ifdef _DEBUG
		if (_yaw != _yaw || _pitch != _pitch || _roll != _roll)
		{
			console::error("Rotation values corrupted!");
		}
#endif
		rotation.x = _yaw;
		rotation.y = _pitch;
		rotation.z = _roll;
		needUpdate = true;
	}

	void Mesh::setRotation(const glm::vec3 &_newRotation)
	{
#ifdef _DEBUG
		if (_newRotation.x != _newRotation.x || _newRotation.y != _newRotation.y || _newRotation.z != _newRotation.z)
		{
			console::error("Position values corrupted!");
		}
#endif
		rotation = _newRotation;
		needUpdate = true;
	}

	void Mesh::setScale(const float &_newScale)
	{
#ifdef _DEBUG
		if (_newScale != _newScale)
		{
			console::error("Scale values corrupted!");
		}
#endif
		scaleX = _newScale;
		scaleY = _newScale;
		scaleZ = _newScale;
		needUpdate = true;
	}

	void Mesh::setScale(const float &_newScaleX, const float &_newScaleY, const float &_newScaleZ)
	{
#ifdef _DEBUG
		if (_newScaleX != _newScaleX || _newScaleY != _newScaleY || _newScaleZ != _newScaleZ)
		{
			console::error("Position values corrupted!");
		}
#endif
		scaleX = _newScaleX;
		scaleY = _newScaleY;
		scaleZ = _newScaleZ;
		needUpdate = true;
	}

	void Mesh::setScale(const glm::vec3 &_newScale)
	{
#ifdef _DEBUG
		if (_newScale.x != _newScale.x || _newScale.y != _newScale.y || _newScale.z != _newScale.z)
		{
			console::error("Position values corrupted!");
		}
#endif
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

	void Mesh::setBlending(const bool &_value)
	{
		blending = _value;
	}


	TextureData* Mesh::setTexture(const std::string &_texturePath)
	{
		TextureData* value = textureManager->getTextureData(_texturePath);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultMesh)
			shaderIndex = DefaultTextureMesh;
		return value;
	}

	TextureData* Mesh::setTexture(const size_t &_textureID)
	{
		TextureData* value = textureManager->getTextureData(_textureID);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultMesh)
			shaderIndex = DefaultTextureMesh;
		return value;
	}

	void Mesh::setTexture(TextureData* _textureDataPtr)
	{
		textureDataID = _textureDataPtr->textureDataID;
		if (shaderIndex == DefaultMesh)
			shaderIndex = DefaultTextureMesh;
	}
}
