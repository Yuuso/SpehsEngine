#include "stdafx.h"

#include "SpehsEngine/Core/BitwiseOperations.h"

#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Batch3D.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/ModelManager.h"
#include "SpehsEngine/Rendering/Camera3D.h"

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
			, localPosition(0.0f, 0.0f, 0.0f)
			, localRotation(glm::vec3(0.0f, 0.0f, 0.0f))
			, localScale(1.0f, 1.0f, 1.0f)
			, meshPosition(0.0f, 0.0f, 0.0f)
			, meshRotation(glm::vec3(0.0f, 0.0f, 0.0f))
			, meshScale(1.0f, 1.0f, 1.0f)
			, position(0.0f, 0.0f, 0.0f)
			, rotation(glm::vec3(0.0f, 0.0f, 0.0f))
			, scale(1.0f, 1.0f, 1.0f)
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
			, name(_other.name)
			, vertexArray(_other.vertexArray)
			, elementArray(_other.elementArray)
		{
#ifdef _DEBUG
			meshAllocations++;
#endif
		}
		void Mesh::setName(const std::string& _name)
		{
			name = _name;
		}

		void Mesh::setVertices(const std::vector<Vertex3D>& _vertices)
		{
			vertexArray = _vertices;
			useMeshColor = false;
			useMeshAlpha = false;
			unbatch();
		}
		void Mesh::setIndices(const std::vector<GLushort>& _indices)
		{
			elementArray = _indices;
			unbatch();
		}

		void Mesh::setBillboarding(const Billboarding _value)
		{
			billboarding = _value;
		}

		void Mesh::setLocalPosition(const glm::vec3& _newPosition)
		{
#ifdef _DEBUG
			if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y || _newPosition.z != _newPosition.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			if (_newPosition == localPosition)
				return;
			localPosition = _newPosition;
			enableBit(needUpdate, VERTEX_UPDATE_VERTEX);
		}
		void Mesh::setLocalRotation(const glm::quat& _newRotation)
		{
#ifdef _DEBUG
			if (_newRotation.x != _newRotation.x || _newRotation.y != _newRotation.y || _newRotation.z != _newRotation.z || _newRotation.w != _newRotation.w)
			{
				log::error("Rotation values corrupted!");
			}
#endif
			if (_newRotation == localRotation)
				return;
			localRotation = _newRotation;
			enableBit(needUpdate, VertexUpdate(VERTEX_UPDATE_VERTEX | VERTEX_UPDATE_NORMAL));
		}
		void Mesh::setLocalScale(const glm::vec3& _newScale)
		{
#ifdef _DEBUG
			if (_newScale.x != _newScale.x || _newScale.y != _newScale.y || _newScale.z != _newScale.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			if (_newScale == localScale)
				return;
			localScale = _newScale;
			enableBit(needUpdate, VERTEX_UPDATE_VERTEX);
		}
		void Mesh::translateLocal(const glm::vec3& _translation)
		{
#ifdef _DEBUG
			if (_translation.x != _translation.x || _translation.y != _translation.y || _translation.z != _translation.z)
			{
				log::error("Translation values corrupted!");
			}
#endif
			if (_translation == glm::vec3(0.0f))
				return;
			localPosition += _translation;
			enableBit(needUpdate, VERTEX_UPDATE_VERTEX);
		}
		void Mesh::updatePosition(const glm::vec3& _newPosition)
		{
#ifdef _DEBUG
			if (_newPosition.x != _newPosition.x || _newPosition.y != _newPosition.y || _newPosition.z != _newPosition.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			if (_newPosition == position)
				return;
			position = _newPosition;
			enableBit(needUpdate, VERTEX_UPDATE_VERTEX);
		}
		void Mesh::updateRotation(const glm::quat& _newRotation)
		{
#ifdef _DEBUG
			if (_newRotation.x != _newRotation.x || _newRotation.y != _newRotation.y || _newRotation.z != _newRotation.z || _newRotation.w != _newRotation.w)
			{
				log::error("Rotation values corrupted!");
			}
#endif
			if (_newRotation == rotation)
				return;
			rotation = _newRotation;
			enableBit(needUpdate, VertexUpdate(VERTEX_UPDATE_VERTEX | VERTEX_UPDATE_NORMAL));
		}
		void Mesh::updateScale(const glm::vec3& _newScale)
		{
#ifdef _DEBUG
			if (_newScale.x != _newScale.x || _newScale.y != _newScale.y || _newScale.z != _newScale.z)
			{
				log::error("Position values corrupted!");
			}
#endif
			if (_newScale == scale)
				return;
			scale = _newScale;
			enableBit(needUpdate, VERTEX_UPDATE_VERTEX);
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
			enableBit(needUpdate, VERTEX_UPDATE_VERTEX);
		}
		void Mesh::setColor(const Color _color)
		{
			se_assert(vertexArray.size() > 0);
			if (vertexArray.size() <= 0)
				return;
			if (useMeshColor && _color == vertexArray[0].color)
				return;
			for (size_t i = 0; i < vertexArray.size(); i++)
			{
				vertexArray[i].color = _color;
			}
			useMeshColor = true;
			useMeshAlpha = true;
			enableBit(needUpdate, VERTEX_UPDATE_COLOR);
		}
		void Mesh::setAlpha(const float _alpha)
		{
			se_assert(vertexArray.size() > 0);
			if (vertexArray.size() <= 0)
				return;
			if (useMeshAlpha && _alpha == vertexArray[0].color.a)
				return;
			for (size_t i = 0; i < vertexArray.size(); i++)
			{
				vertexArray[i].color.a = _alpha;
			}
			useMeshAlpha = true;
			enableBit(needUpdate, VERTEX_UPDATE_COLOR);
		}
		void Mesh::setColor(const size_t _index, const Color _color)
		{
			se_assert(vertexArray.size() > _index);
			if (vertexArray.size() <= _index)
				return;
			vertexArray[_index].color = _color;
			useMeshColor = false;
			useMeshAlpha = false;
			enableBit(needUpdate, VERTEX_UPDATE_COLOR);
		}
		void Mesh::setAlpha(const size_t _index, const float _alpha)
		{
			se_assert(vertexArray.size() > _index);
			if (vertexArray.size() <= _index)
				return;
			vertexArray[_index].color.a = _alpha;
			useMeshAlpha = false;
			enableBit(needUpdate, VERTEX_UPDATE_COLOR);
		}
		void Mesh::setBlending(const bool _value)
		{
			if (_value == blending)
				return;
			blending = _value;
			unbatch();
		}
		void Mesh::setDepthTest(const bool _value)
		{
			if (_value == depthTest)
				return;
			depthTest = _value;
			unbatch();
		}
		void Mesh::setRenderState(const bool _newState)
		{
			if (_newState == renderState)
				return;
			renderState = _newState;
			unbatch();
		}
		void Mesh::toggleRenderState()
		{
			renderState = !renderState;
			unbatch();
		}
		void Mesh::setShaderIndex(const unsigned int _newShaderIndex)
		{
			if (_newShaderIndex == shaderIndex)
				return;
			shaderIndex = _newShaderIndex;
			unbatch();
		}
		void Mesh::setBackFaceCulling(const bool _value)
		{
			if (_value == backFaceCulling)
				return;
			backFaceCulling = _value;
			unbatch();
		}
		void Mesh::setLineWidth(const float _value)
		{
			if (_value == lineWidth)
				return;
			lineWidth = _value;
			unbatch();
		}
		void Mesh::setLineSmoothing(const bool _value)
		{
			if (_value == smoothLine)
				return;
			smoothLine = _value;
			unbatch();
		}
		void Mesh::setStaticDraw(const bool _value)
		{
			if (_value == staticDraw)
				return;
			staticDraw = _value;
			unbatch();
		}
		void Mesh::setTexture(TextureManager& _textureManager, const std::string& _texturePath, const size_t _index)
		{
			if (TextureData* value = _textureManager.getTextureData(_texturePath))
			{
				setTexture(*value, _index);
			}
			else if (const TextureData* defaultTexture = _textureManager.getDefaultTexture())
			{
				setTexture(*defaultTexture, _index);
			}
		}
		void Mesh::setTexture(TextureManager& _textureManager, const size_t _textureID, const size_t _index)
		{
			if (TextureData* value = _textureManager.getTextureData(_textureID))
			{
				setTexture(*value, _index);
			}
			else if (const TextureData* defaultTexture = _textureManager.getDefaultTexture())
			{
				setTexture(*defaultTexture, _index);
			}
		}
		void Mesh::setTexture(const TextureData& _textureData, const size_t _index)
		{
			if (textureDataIDs.size() < (_index + 1))
				textureDataIDs.resize(_index + 1);
			else if (_textureData.textureDataID == textureDataIDs[_index])
				return;
			textureDataIDs[_index] = _textureData.textureDataID;
			unbatch();
		}

		const se::Color& Mesh::getColor() const
		{
			if (!useMeshColor) se::log::warning("Mesh color may not be accurate!");
			return getColor(0);
		}
		float Mesh::getAlpha() const
		{
			if (!useMeshAlpha) se::log::warning("Mesh alpha may not be accurate!");
			return getAlpha(0);
		}
		const se::Color& Mesh::getColor(const size_t _index) const
		{
			se_assert(_index < vertexArray.size());
			return vertexArray[_index].color;
		}
		float Mesh::getAlpha(const size_t _index) const
		{
			se_assert(_index < vertexArray.size());
			return vertexArray[_index].color.a;
		}

		void Mesh::unbatch()
		{
			for (size_t i = 0; i < batchManagers.size(); i++)
				batchManagers[i]->unbatchMesh(*this);
		}
	}
}
