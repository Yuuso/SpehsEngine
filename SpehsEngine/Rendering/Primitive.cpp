#include "stdafx.h"

#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Batch.h"

#include <GL/glew.h>

#include <atomic>



std::atomic<int> primitiveAllocations;
std::atomic<int> primitiveDeallocations;


namespace se
{
	namespace rendering
	{
		Primitive::Primitive(BatchManager& _batchManager)
			: batchManager(_batchManager)
		{
#ifdef _DEBUG
			primitiveAllocations++;
#endif
			shaderIndex = (int)ShaderName::DefaultPolygon;
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
				log::error("Position values corrupted!");
			}
#endif
			position.x = _x;
			position.y = _y;
			needUpdate = true;
		}

		void Primitive::setPosition(const glm::vec2& _newPosition)
		{
#ifdef _DEBUG
			if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y)
			{
				log::error("Position values corrupted!");
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
				log::error("Position values corrupted!");
			}
#endif
			position.x = _other.position.x;
			position.y = _other.position.y;
			needUpdate = true;
		}

		void Primitive::translate(const float _x, const float _y)
		{
#ifdef _DEBUG
			if (_x != _x || _y != _y)
			{
				log::error("Position values corrupted!");
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
				log::error("Position values corrupted!");
			}
#endif
			position.x += _translation.x;
			position.y += _translation.y;
			needUpdate = true;
		}

		void Primitive::setScale(const float _newScale)
		{
#ifdef _DEBUG
			if (_newScale != _newScale)
			{
				log::error("Scale values corrupted!");
			}
#endif
			scaleX = _newScale;
			scaleY = _newScale;
			needUpdate = true;
		}

		void Primitive::setScale(const float _newScaleX, const float _newScaleY)
		{
#ifdef _DEBUG
			if (_newScaleX != _newScaleX || _newScaleY != _newScaleY)
			{
				log::error("Scale values corrupted!");
			}
#endif
			scaleX = _newScaleX;
			scaleY = _newScaleY;
			needUpdate = true;
		}

		void Primitive::setScale(const glm::vec2& _newScale)
		{
#ifdef _DEBUG
			if (_newScale.x != _newScale.x || _newScale.y != _newScale.y)
			{
				log::error("Scale values corrupted!");
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
				log::error("Scale value corrupted!");
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
				log::error("Scale value corrupted!");
			}
#endif
			scaleY = _newScaleY;
			needUpdate = true;
		}

		void Primitive::setRotation(const float _newRotation)
		{
#ifdef _DEBUG
			if (_newRotation != _newRotation)
			{
				log::error("Rotation values corrupted!");
			}
#endif
			rotation = _newRotation;
			needUpdate = true;
		}

		void Primitive::setRotation(const float _newRotation, const glm::vec3& _newRotationVector)
		{
#ifdef _DEBUG
			if (_newRotation != _newRotation || _newRotationVector.x != _newRotationVector.x || _newRotationVector.y != _newRotationVector.y || _newRotationVector.z != _newRotationVector.z)
			{
				log::error("Rotation values corrupted!");
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
				log::error("Rotation values corrupted!");
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
				log::error("Rotation values corrupted!");
			}
#endif
			rotation += _rotation;
			needUpdate = true;
		}

		void Primitive::setRotationVector(const glm::vec3& _newRotationVector)
		{
#ifdef _DEBUG
			if (_newRotationVector.x != _newRotationVector.x || _newRotationVector.y != _newRotationVector.y || _newRotationVector.z != _newRotationVector.z)
			{
				log::error("Rotation values corrupted!");
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

		void Primitive::setColor(const se::Color& _newColor)
		{
			primitiveColor = _newColor;
			for (unsigned i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].color = primitiveColor;
			}
		}

		void Primitive::setAlpha(const float _alpha)
		{
			primitiveColor.a = _alpha;
			for (unsigned i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].color.a = primitiveColor.a;
			}
		}

		void Primitive::setCameraMatrixState(const bool _newState)
		{
			cameraMatrixState = _newState;
		}

		void Primitive::setPlaneDepth(const PlaneDepth _newPlaneDepth)
		{
#ifdef _DEBUG
			if (_newPlaneDepth != _newPlaneDepth)
			{
				log::error("Depth values corrupted!");
			}
#endif
			planeDepth = _newPlaneDepth;
		}

		void Primitive::setLineWidth(const float _newWidth)
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

		void Primitive::setShaderIndex(const unsigned int _newShaderIndex)
		{
			shaderIndex = _newShaderIndex;
		}

		void Primitive::setBlending(const bool _newState)
		{
			blending = _newState;
		}
	}
}
