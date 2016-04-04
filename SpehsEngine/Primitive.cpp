
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
		shaderIndex = DefaultPolygon;
		drawMode = UNDEFINED;
		numVertices = 0;
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
		vertexArray = nullptr;
		worldVertexArray = nullptr;
	}
	Primitive::~Primitive()
	{
		if (worldVertexArray != nullptr)
			delete [] worldVertexArray;
		if (vertexArray != nullptr)
			delete [] vertexArray;
	}

	void Primitive::destroy()
	{
		readyForDelete = true;
	}

	void Primitive::setPosition(const float &_x, const float &_y)
	{
		position.setPosition(_x, _y);
	}

	void Primitive::setPosition(const float &_x, const float &_y, const float &_z)
	{
		position.setPosition(_x, _y, _z);
	}

	void Primitive::setPosition(const glm::vec2 &_newPosition)
	{
		position.setPosition(_newPosition.x, _newPosition.y);
	}

	void Primitive::setPosition(const glm::vec3 &_newPosition)
	{
		position.setPosition(_newPosition.x, _newPosition.y, _newPosition.z);
	}

	void Primitive::setPosition(const Position &_newPosition)
	{
		position = _newPosition;
	}

	void Primitive::setScale(const float &_newScale)
	{
		scaleX = _newScale;
		scaleY = _newScale;
	}

	void Primitive::setScale(const float &_newScaleX, const float &_newScaleY)
	{
		scaleX = _newScaleX;
		scaleY = _newScaleY;
	}

	void Primitive::setScale(const glm::vec2 &_newScale)
	{
		scaleX = _newScale.x;
		scaleY = _newScale.y;
	}

	void Primitive::setRotation(const float &_newRotation)
	{
		rotation = _newRotation;
	}

	void Primitive::setRotation(const float &_newRotation, const glm::vec3 &_newRotationVector)
	{
		rotation = _newRotation;
		rotationVector = _newRotationVector;
	}

	void Primitive::setRotationVector(const glm::vec3 &_newRotationVector)
	{
		rotationVector = _newRotationVector;
	}

	void Primitive::setColor(const glm::vec4 &_newColor)
	{
		primitiveColor = _newColor;
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColor(const glm::vec3 &_newColor)
	{
		primitiveColor = glm::vec4(_newColor, 1.0f);
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a)
	{
		primitiveColor = glm::vec4((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f);
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.setColor(primitiveColor);
		}
	}

	void Primitive::setColorAlpha(const float &_a)
	{
		primitiveColor.a = _a;
		for (unsigned i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].color.rgba.a = primitiveColor.a;
		}
	}

	void Primitive::setColorAlpha(const unsigned char _a)
	{
		primitiveColor.a = (float) _a / 255.0f;
		for (unsigned i = 0; i < numVertices; i++)
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
