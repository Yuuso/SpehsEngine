
#include "Primitive.h"
#include "Vertex.h"
#include "ShaderManager.h"
#include "Console.h"
#include "Batch.h"

#include <GL/glew.h>


namespace SpehsEngine
{
	Primitive::Primitive()
	{
		cameraMatrixState = true;
		planeDepth = 0;
		rotation = 0.0f;
		scaleX = 0.0f;
		scaleY = 0.0f;
		rotationVector = glm::vec3(0.0f);
		primitiveColor = glm::vec4(1.0f);
	}
	Primitive::~Primitive()
	{

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

	void Primitive::setRotation(const float &_newRotation, const glm::vec3 &_newRotationVector)
	{
		rotation = _newRotation;
		rotationVector = _newRotationVector;
	}

	void Primitive::setColor(const glm::vec4 &_newColor)
	{
		useColor = true;
		primitiveColor = _newColor;
	}

	void Primitive::setColor(const glm::vec3 &_newColor)
	{
		useColor = true;
		primitiveColor = glm::vec4(_newColor, 1.0f);
	}

	void Primitive::setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a)
	{
		useColor = true;
		primitiveColor = glm::vec4((float) _r / 255.0f, (float) _g / 255.0f, (float) _b / 255.0f, (float) _a / 255.0f);
	}

	void Primitive::setColorAlpha(const float &_a)
	{
		if (!useColor)
		{
			console->error("Must set color before modifying alpha!");
			return;
		}
		primitiveColor.a = _a;
	}

	void Primitive::setColorAlpha(const unsigned char &_a)
	{
		if (!useColor)
		{
			console->error("Must set color before modifying alpha!");
			return;
		}
		primitiveColor.a = (float)_a / 255.0f;
	}

	void Primitive::setCameraMatrixState(const bool _newState)
	{
		cameraMatrixState = _newState;
	}

	void Primitive::setPlaneDepth(const PlaneDepth &_newPlaneDepth)
	{
		planeDepth = _newPlaneDepth;
		//TODO: Move to another Batch! (yeah this function doesn't really do anything useful atm..)
	}
}
