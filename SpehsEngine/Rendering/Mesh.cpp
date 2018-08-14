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
			, drawMode(GL_TRIANGLES)
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

		Mesh::Mesh(const Mesh& _other)
			: backFaceCulling(_other.backFaceCulling)
			, renderState(_other.renderState)
			, blending(_other.blending)
			, depthTest(_other.depthTest)
			, shaderIndex(_other.shaderIndex)
			, textureDataIDs(_other.textureDataIDs)
			, drawMode(_other.drawMode)
			, position(_other.position)
			, rotation(_other.rotation)
			, scale(_other.scale)
			, color(_other.color)
			, vertexArray(_other.vertexArray)
			, elementArray(_other.elementArray)
		{
#ifdef _DEBUG
			meshAllocations++;
#endif
		}
		void Mesh::setVertices(const std::vector<Vertex3D>& _vertices)
		{
			vertexArray = _vertices;
		}
		void Mesh::setIndices(const std::vector<GLushort>& _indices)
		{
			elementArray = _indices;
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
		void Mesh::setRotation(const glm::quat& _newRotation)
		{
#ifdef _DEBUG
			if (_newRotation.x != _newRotation.x || _newRotation.y != _newRotation.y || _newRotation.z != _newRotation.z || _newRotation.w != _newRotation.w)
			{
				log::error("Rotation values corrupted!");
			}
#endif
			rotation = _newRotation;
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
		void Mesh::setColor(const Color _color)
		{
			color = _color;
			needUpdate = true;
		}
		void Mesh::setAlpha(const float _alpha)
		{
			color.a = _alpha;
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
		void Mesh::setTexture(const std::string& _texturePath, const size_t _index)
		{
			if (!batchManager)
				log::error("Cannot load texture without batchmanager!");
			TextureData* value = batchManager->textureManager.getTextureData(_texturePath);
			setTexture(value, _index);
		}
		void Mesh::setTexture(const size_t _textureID, const size_t _index)
		{
			if (!batchManager)
				log::error("Cannot load texture without batchmanager!");
			TextureData* value = batchManager->textureManager.getTextureData(_textureID);
			setTexture(value, _index);
		}
		void Mesh::setTexture(TextureData* _textureDataPtr, const size_t _index)
		{
			if (textureDataIDs.size() >= _index)
				textureDataIDs.resize(_index + 1);
			textureDataIDs[_index] = _textureDataPtr->textureDataID;
		}
	}
}
