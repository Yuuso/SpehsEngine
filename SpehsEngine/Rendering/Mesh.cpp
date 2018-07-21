#include "stdafx.h"

#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Batch3D.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/ModelManager.h"

#include <GL/glew.h>

#include <atomic>


#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif


std::atomic<int> meshAllocations;
std::atomic<int> meshDeallocations;


namespace se
{
	namespace rendering
	{
		Mesh::Mesh()
			: shaderIndex((unsigned int)ShaderName::DefaultMesh)
		{
#ifdef _DEBUG
			meshAllocations++;
#endif
		}
		Mesh::~Mesh()
		{
#ifdef _DEBUG
			meshDeallocations++;
#endif
		}

		void Mesh::setPosition(const float _x, const float _y, const float _z)
		{
#ifdef _DEBUG
			if (_x != _x || _y != _y || _z != _z)
			{
				log::error("Position values corrupted!");
			}
#endif
			position.x = _x;
			position.y = _y;
			position.z = _z;
			needUpdate = true;
		}
		void Mesh::setPosition(const glm::vec3& _newPosition)
		{
#ifdef _DEBUG
			if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y || _newPosition.z != _newPosition.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			position = _newPosition;
			needUpdate = true;
		}
		void Mesh::setPosition(const Mesh& _other)
		{
#ifdef _DEBUG
			if (_other.position.x != _other.position.x || _other.position.y != _other.position.y || _other.position.z != _other.position.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			position = _other.position;
			needUpdate = true;
		}
		void Mesh::setRotation(const float _yaw, const float _pitch, const float _roll)
		{
#ifdef _DEBUG
			if (_yaw != _yaw || _pitch != _pitch || _roll != _roll)
			{
				log::error("Rotation values corrupted!");
			}
#endif
			rotation.x = _yaw;
			rotation.y = _pitch;
			rotation.z = _roll;
			needUpdate = true;
		}
		void Mesh::setRotation(const glm::vec3& _newRotation)
		{
#ifdef _DEBUG
			if (_newRotation.x != _newRotation.x || _newRotation.y != _newRotation.y || _newRotation.z != _newRotation.z)
			{
				log::error("Rotation values corrupted!");
			}
#endif
			rotation = _newRotation;
			needUpdate = true;
		}
		void Mesh::setScale(const float _newScale)
		{
#ifdef _DEBUG
			if (_newScale != _newScale)
			{
				log::error("Scale values corrupted!");
			}
#endif
			scale.x = _newScale;
			scale.y = _newScale;
			scale.z = _newScale;
			needUpdate = true;
		}
		void Mesh::setScale(const float _newScaleX, const float _newScaleY, const float _newScaleZ)
		{
#ifdef _DEBUG
			if (_newScaleX != _newScaleX || _newScaleY != _newScaleY || _newScaleZ != _newScaleZ)
			{
				log::error("Scale values corrupted!");
			}
#endif
			scale.x = _newScaleX;
			scale.y = _newScaleY;
			scale.z = _newScaleZ;
			needUpdate = true;
		}
		void Mesh::setScale(const glm::vec3& _newScale)
		{
#ifdef _DEBUG
			if (_newScale.x != _newScale.x || _newScale.y != _newScale.y || _newScale.z != _newScale.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			scale = _newScale;
			needUpdate = true;
		}
		void Mesh::setColor(const Mesh& _other)
		{
			color = _other.color;
		}
		void Mesh::setColor(const Color _color)
		{
			color = _color;
		}
		void Mesh::setAlpha(const float _alpha)
		{
			color.a = _alpha;
		}
		void Mesh::translate(const float _x, const float _y, const float _z)
		{
#ifdef _DEBUG
			if (_x != _x || _y != _y || _z != _z)
			{
				log::error("Position values corrupted!");
			}
#endif
			position.x += _x;
			position.y += _y;
			position.z += _z;
			needUpdate = true;
		}
		void Mesh::translate(const glm::vec3& _translation)
		{
#ifdef _DEBUG
			if (_translation.x != _translation.x || _translation.y != _translation.y || _translation.z != _translation.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			position += _translation;
			needUpdate = true;
		}
		void Mesh::setBlending(const bool _value)
		{
			blending = _value;
		}
		void Mesh::setDepthTest(const bool _value)
		{
			depthTest = _value;
		}
		void Mesh::setRenderState(const Mesh& _other)
		{
			renderState = _other.renderState;
		}
		void Mesh::setRenderState(const bool _newState)
		{
			renderState = _newState;
		}
		void Mesh::toggleRenderState()
		{
			renderState = !renderState;
		}
		void Mesh::setShaderIndex(const unsigned int _newShaderIndex)
		{
			shaderIndex = _newShaderIndex;
		}
		void Mesh::setBackFaceCulling(const bool _value)
		{
			backFaceCulling = _value;
		}
		TextureData* Mesh::setTexture(const std::string& _texturePath)
		{
			if (!batchManager)
				log::error("Cannot load texture without batchmanager!");
			TextureData* value = batchManager->textureManager.getTextureData(_texturePath);
			textureDataID = value->textureDataID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultMesh)
				shaderIndex = (unsigned int)ShaderName::DefaultTextureMesh;
			return value;
		}
		TextureData* Mesh::setTexture(const size_t _textureID)
		{
			if (!batchManager)
				log::error("Cannot load texture without batchmanager!");
			TextureData* value = batchManager->textureManager.getTextureData(_textureID);
			textureDataID = value->textureDataID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultMesh)
				shaderIndex = (unsigned int)ShaderName::DefaultTextureMesh;
			return value;
		}
		void Mesh::setTexture(TextureData* _textureDataPtr)
		{
			textureDataID = _textureDataPtr->textureDataID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultMesh)
				shaderIndex = (unsigned int)ShaderName::DefaultTextureMesh;
		}
	}
}
