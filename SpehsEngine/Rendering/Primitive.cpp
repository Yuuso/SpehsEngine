
#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Batch.h"

#include <GL/glew.h>

#include <atomic>

#ifdef _DEBUG //Debug includes
#include "SpehsEngine/Rendering/BatchManager.h"
#endif



std::atomic<int> primitiveAllocations;
std::atomic<int> primitiveDeallocations;


namespace spehs
{
	Primitive::Primitive(BatchManager& _batchManager)
		: batchManager(_batchManager)
		, readyForDelete(false)
		, blending(false)
		, renderState(true)
		, cameraMatrixState(true)
		, needUpdate(true)
		, textureDataID(0)
		, planeDepth(0)
		, drawMode(UNDEFINED)
		, shaderIndex(DefaultPolygon)
		, lineWidth(0.0f)
		, rotation(0.0f)
		, scaleX(1.0f)
		, scaleY(1.0f)
		, rotationVector(0.0f, 0.0f, 1.0f)//?
		, primitiveColor(255, 255, 255, 255)
		, scaledMatrix(1.0f)
		, scaledRotatedMatrix(1.0f)
		, position(0.0f, 0.0f)
	{
#ifdef _DEBUG
		primitiveAllocations++;
#endif
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
		position.x = _x;
		position.y = _y;
		needUpdate = true;
	}

	void Primitive::setPosition(const spehs::vec2 &_newPosition)
	{
#ifdef _DEBUG
		if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y)
		{
			exceptions::unexpectedError("Position values corrupted!");
		}
#endif
		position.x = _newPosition.x;
		position.y = _newPosition.y;
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

	void Primitive::translate(const spehs::vec2& _translation)
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

	void Primitive::setScale(const spehs::vec2 &_newScale)
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

	void Primitive::setRotation(const float &_newRotation, const spehs::vec3 &_newRotationVector)
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

	void Primitive::setRotationVector(const spehs::vec3 &_newRotationVector)
	{
#ifdef _DEBUG
		if (_newRotationVector.x != _newRotationVector.x || _newRotationVector.y != _newRotationVector.y || _newRotationVector.z != _newRotationVector.z)
		{
			exceptions::unexpectedError("Rotation values corrupted!");
		}
#endif
		rotationVector = _newRotationVector;
	}
	
	void Primitive::setColor(const Primitive& other)
	{
		primitiveColor = other.primitiveColor;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color = primitiveColor;
		}
	}

	void Primitive::setColor(const spehs::Color& _newColor)
	{
		primitiveColor = _newColor;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color = primitiveColor;
		}
	}

	void Primitive::setAlpha(const spehs::Color::Component& _newAlpha)
	{
		primitiveColor.a = _newAlpha;
		for (unsigned i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].color.a = primitiveColor.a;
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
		if (batchManager.shaderManager.getShader(_newShaderIndex) == nullptr)
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
