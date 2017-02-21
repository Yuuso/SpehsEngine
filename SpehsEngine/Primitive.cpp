
#include "Primitive.h"
#include "Vertex.h"
#include "ShaderManager.h"
#include "Exceptions.h"
#include "Batch.h"

#include <GL/glew.h>

#ifdef _DEBUG //Debug includes
#include "BatchManager.h"
#endif



int64_t primitiveAllocations;
int64_t primitiveDeallocations;


namespace spehs
{
	Primitive::Primitive()
#ifdef _DEBUG
		: myBatchManager(getActiveBatchManager())
#endif
	{
#ifdef _DEBUG
		primitiveAllocations++;
#endif

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
#ifdef _DEBUG
		primitiveDeallocations++;
#endif
	}

	void Primitive::destroy()
	{
		readyForDelete = true;
	}

	void Primitive::setPosition(const float _x, const float _y)
	{
#ifdef _DEBUG
		if (_x != _x || _y != _y)
		{
			exceptions::unexpectedError("Position values corrupted!");
		}
#endif
		position = glm::vec2(_x, _y);
		needUpdate = true;
	}

	void Primitive::setPosition(const glm::vec2 &_newPosition)
	{
#ifdef _DEBUG
		if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y)
		{
			exceptions::unexpectedError("Position values corrupted!");
		}
#endif
		position = glm::vec2(_newPosition.x, _newPosition.y);
		needUpdate = true;
	}

	void Primitive::setPosition(const Primitive& _other)
	{
#ifdef _DEBUG
		if (_other.position.x != _other.position.x || _other.position.y != _other.position.y)
		{
			exceptions::unexpectedError("Position values corrupted!");
		}
#endif
		position.x = _other.position.x;
		position.y = _other.position.y;
		needUpdate = true;
	}

	void Primitive::translate(const float &_x, const float &_y)
	{
#ifdef _DEBUG
		if (_x != _x || _y != _y)
		{
			exceptions::unexpectedError("Position values corrupted!");
		}
#endif
		position.x += _x;
		position.y += _y;
		needUpdate = true;
	}

	void Primitive::translate(const glm::vec2& _translation)
	{
#ifdef _DEBUG
		if (_translation.x != _translation.x || _translation.y != _translation.y)
		{
			exceptions::unexpectedError("Position values corrupted!");
		}
#endif
		position.x += _translation.x;
		position.y += _translation.y;
		needUpdate = true;
	}

	void Primitive::setScale(const float &_newScale)
	{
#ifdef _DEBUG
		if (_newScale != _newScale)
		{
			exceptions::unexpectedError("Scale values corrupted!");
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
			exceptions::unexpectedError("Scale values corrupted!");
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
			exceptions::unexpectedError("Scale values corrupted!");
		}
#endif
		scaleX = _newScale.x;
		scaleY = _newScale.y;
		needUpdate = true;
	}

	void Primitive::setScaleX(const float _newScaleX)
	{
#ifdef _DEBUG
		if (_newScaleX != _newScaleX)
		{
			exceptions::unexpectedError("Scale value corrupted!");
		}
#endif
		scaleX = _newScaleX;
		needUpdate = true;
	}

	void Primitive::setScaleY(const float _newScaleY)
	{
#ifdef _DEBUG
		if (_newScaleY != _newScaleY)
		{
			exceptions::unexpectedError("Scale value corrupted!");
		}
#endif
		scaleY = _newScaleY;
		needUpdate = true;
	}

	void Primitive::setRotation(const float &_newRotation)
	{
#ifdef _DEBUG
		if (_newRotation != _newRotation)
		{
			exceptions::unexpectedError("Rotation values corrupted!");
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
			exceptions::unexpectedError("Rotation values corrupted!");
		}
#endif
		rotation = _newRotation;
		rotationVector = _newRotationVector;
		needUpdate = true;
	}

	void Primitive::setRotation(const Primitive& _other)
	{
#ifdef _DEBUG
		if (_other.rotation != _other.rotation)
		{
			exceptions::unexpectedError("Rotation values corrupted!");
		}
#endif
		rotation = _other.rotation;
		needUpdate = true;
	}

	void Primitive::rotate(const float _rotation)
	{
#ifdef _DEBUG
		if (_rotation != _rotation)
		{
			exceptions::unexpectedError("Rotation values corrupted!");
		}
#endif
		rotation += _rotation;
		needUpdate = true;
	}

	void Primitive::setRotationVector(const glm::vec3 &_newRotationVector)
	{
#ifdef _DEBUG
		if (_newRotationVector.x != _newRotationVector.x || _newRotationVector.y != _newRotationVector.y || _newRotationVector.z != _newRotationVector.z)
		{
			exceptions::unexpectedError("Rotation values corrupted!");
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
			exceptions::unexpectedError("Invalid CommonColor: " + std::to_string(_color));
			break;
		}
	}

	void Primitive::setColor(const unsigned char* _rgbaArray)
	{
		primitiveColor = glm::vec4((float)_rgbaArray[0] / 255.0f, (float)_rgbaArray[1] / 255.0f, (float)_rgbaArray[2] / 255.0f, (float)_rgbaArray[3] / 255.0f);
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}
	
	void Primitive::setColor(const Primitive& other)
	{
		primitiveColor = other.primitiveColor;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColorAlpha(const float _a)
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
			exceptions::unexpectedError("Depth values corrupted!");
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

	void Primitive::setRenderState(const Primitive& _other)
	{
		renderState = _other.renderState;
	}

	void Primitive::setShader(const int &_newShaderIndex)
	{
		if (shaderManager->getShader(_newShaderIndex) == nullptr)
		{
			exceptions::unexpectedError("Trying to set a non-existing shader to Primitive!");
			return;
		}
		shaderIndex = _newShaderIndex;
	}

	void Primitive::setBlending(const bool _newState)
	{
		blending = _newState;
	}
}
