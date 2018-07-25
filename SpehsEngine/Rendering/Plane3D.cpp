
#include "stdafx.h"

#include "SpehsEngine/Rendering/Plane3D.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Vertex3D.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>


namespace se
{
	namespace rendering
	{
		Plane3D::Plane3D()
		{
			mesh = new Mesh();
			mesh->setBackFaceCulling(false);
			mesh->vertexArray = {
				Vertex3D(glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
				Vertex3D(glm::vec3(0.5f, 0.0f, 0.5f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
				Vertex3D(glm::vec3(0.5f, 0.0f, -0.5f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
				Vertex3D(glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))
			};
			mesh->elementArray = { 0, 3, 2, 0, 2, 1 };
		}
		Plane3D::~Plane3D()
		{
			if (batchManager)
				batchManager->remove(*this);
			delete mesh;
		}

		void Plane3D::setPosition(const glm::vec3& _newPosition)
		{
			mesh->setPosition(_newPosition);
		}
		void Plane3D::setRotation(const glm::quat& _newRotation)
		{
			mesh->setRotation(_newRotation);
		}
		void Plane3D::setScale(const glm::vec3& _newScale)
		{
			mesh->setScale(_newScale);
		}
		void Plane3D::setColor(const Color _color)
		{
			mesh->setColor(_color);
		}
		void Plane3D::setAlpha(const float _alpha)
		{
			mesh->setAlpha(_alpha);
		}
		void Plane3D::translate(const glm::vec3& _translation)
		{
			mesh->translate(_translation);
		}
		void Plane3D::setBlending(const bool _value)
		{
			mesh->setBlending(_value);
		}
		void Plane3D::setDepthTest(const bool _value)
		{
			mesh->setDepthTest(_value);
		}
		void Plane3D::setRenderState(const bool _newState)
		{
			mesh->setRenderState(_newState);
		}
		void Plane3D::toggleRenderState()
		{
			mesh->toggleRenderState();
		}
		void Plane3D::setShaderIndex(const unsigned int _newShaderIndex)
		{
			mesh->setShaderIndex(_newShaderIndex);
		}
		void Plane3D::setTexture(const std::string& _texturePath, const size_t _index)
		{
			mesh->setTexture(_texturePath, _index);
		}
		void Plane3D::setTexture(const size_t _textureID, const size_t _index)
		{
			mesh->setTexture(_textureID, _index);
		}
		void Plane3D::setTexture(TextureData* _textureDataPtr, const size_t _index)
		{
			mesh->setTexture(_textureDataPtr, _index);
		}

		glm::vec3 Plane3D::getPosition() const
		{
			return mesh->getPosition();
		}
		glm::quat Plane3D::getRotation() const
		{
			return mesh->getRotation();
		}
		glm::vec3 Plane3D::getScale() const
		{
			return mesh->getScale();
		}
		bool Plane3D::getRenderState() const
		{
			return mesh->getRenderState();
		}
		bool Plane3D::getBlending() const
		{
			return mesh->getBlending();
		}
		bool Plane3D::getDepthTest() const
		{
			return mesh->getDepthTest();
		}
		unsigned int Plane3D::getShaderIndex() const
		{
			return mesh->getShaderIndex();
		}
		se::Color Plane3D::getColor() const
		{
			return mesh->getColor();
		}
		float Plane3D::getAlpha() const
		{
			return mesh->getAlpha();
		}
	}
}