
#include "Primitive.h"
#include "Vertex.h"
#include "ShaderManager.h"
#include "Console.h"
#include "Batch.h"

#include <GL/glew.h>


namespace spehs
{
	Primitive::Primitive()
	{
		blending = false;
		cameraMatrixState = true;
		readyForDelete = false;
		renderState = true;
		needUpdate = true;
		shaderIndex = DefaultPolygon;
		drawMode = UNDEFINED;
		textureDataID = 0;
		planeDepth = 0;
		lineWidth = 0.0f;
		rotation = 0.0f;
		scaleX = 1.0f;
		scaleY = 1.0f;
		rotationVector = glm::vec3(0.0f, 0.0f, 1.0f);//?
		primitiveColor = glm::vec4(1.0f);
		scaledMatrix = glm::mat4(1.0f);
		scaledRotatedMatrix = glm::mat4(1.0f);
	}
	Primitive::~Primitive()
	{
	}

	void Primitive::destroy()
	{
		readyForDelete = true;
	}

	void Primitive::setPosition(const float &_x, const float &_y)
	{
#ifdef _DEBUG
		if (_x != _x || _y != _y)
		{
			console::error("Position values corrupted!");
		}
#endif
		position.setPosition(_x, _y);
		needUpdate = true;
	}

	void Primitive::setPosition(const float &_x, const float &_y, const float &_z)
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

	void Primitive::setPosition(const glm::vec2 &_newPosition)
	{
#ifdef _DEBUG
		if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y)
		{
			console::error("Position values corrupted!");
		}
#endif
		position.setPosition(_newPosition.x, _newPosition.y);
		needUpdate = true;
	}

	void Primitive::setPosition(const glm::vec3 &_newPosition)
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

	void Primitive::setPosition(const Position &_newPosition)
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

	void Primitive::setScale(const float &_newScale)
	{
#ifdef _DEBUG
		if (_newScale != _newScale)
		{
			console::error("Scale values corrupted!");
		}
#endif
		scaleX = _newScale;
		scaleY = _newScale;
		needUpdate = true;
	}

	void Primitive::setScale(const float &_newScaleX, const float &_newScaleY)
	{
#ifdef _DEBUG
		if (_newScaleX != _newScaleX || _newScaleY != _newScaleY)
		{
			console::error("Scale values corrupted!");
		}
#endif
		scaleX = _newScaleX;
		scaleY = _newScaleY;
		needUpdate = true;
	}

	void Primitive::setScale(const glm::vec2 &_newScale)
	{
#ifdef _DEBUG
		if (_newScale.x != _newScale.x || _newScale.y != _newScale.y)
		{
			console::error("Scale values corrupted!");
		}
#endif
		scaleX = _newScale.x;
		scaleY = _newScale.y;
		needUpdate = true;
	}

	void Primitive::setRotation(const float &_newRotation)
	{
#ifdef _DEBUG
		if (_newRotation != _newRotation)
		{
			console::error("Rotation values corrupted!");
		}
#endif
		rotation = _newRotation;
		needUpdate = true;
	}

	void Primitive::setRotation(const float &_newRotation, const glm::vec3 &_newRotationVector)
	{
#ifdef _DEBUG
		if (_newRotation != _newRotation || _newRotationVector.x != _newRotationVector.x || _newRotationVector.y != _newRotationVector.y || _newRotationVector.z != _newRotationVector.z)
		{
			console::error("Rotation values corrupted!");
		}
#endif
		rotation = _newRotation;
		rotationVector = _newRotationVector;
		needUpdate = true;
	}

	void Primitive::setRotationVector(const glm::vec3 &_newRotationVector)
	{
#ifdef _DEBUG
		if (_newRotationVector.x != _newRotationVector.x || _newRotationVector.y != _newRotationVector.y || _newRotationVector.z != _newRotationVector.z)
		{
			console::error("Rotation values corrupted!");
		}
#endif
		rotationVector = _newRotationVector;
	}

	void Primitive::setColor(const glm::vec4 &_newColor)
	{
		primitiveColor = _newColor;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColor(const glm::vec3 &_newColor)
	{
		primitiveColor = glm::vec4(_newColor, 1.0f);
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a)
	{
		primitiveColor = glm::vec4((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f);
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColor(const CommonColor& _color)
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

	void Primitive::setColorAlpha(const float &_a)
	{
		primitiveColor.a = _a;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.rgba.a = primitiveColor.a;
		}
	}

	void Primitive::setColorAlpha(const unsigned char _a)
	{
		primitiveColor.a = (float) _a / 255.0f;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.rgba.a = primitiveColor.a;
		}
	}

	void Primitive::setCameraMatrixState(const bool _newState)
	{
		cameraMatrixState = _newState;
	}

	void Primitive::setPlaneDepth(const PlaneDepth &_newPlaneDepth)
	{
#ifdef _DEBUG
		if (_newPlaneDepth != _newPlaneDepth)
		{
			console::error("Depth values corrupted!");
		}
#endif
		planeDepth = _newPlaneDepth;
	}

	void Primitive::setLineWidth(const float &_newWidth)
	{
		lineWidth = _newWidth;
	}

	void Primitive::setRenderState(const bool _newState)
	{
		renderState = _newState;
	}

	void Primitive::setShader(const int &_newShaderIndex)
	{
		if (shaderManager->getShader(_newShaderIndex) == nullptr)
		{
			console::error("Trying to set a non-existing shader to Primitive!");
			return;
		}
		shaderIndex = _newShaderIndex;
	}

	void Primitive::setBlending(const bool _newState)
	{
		blending = _newState;
	}
}
