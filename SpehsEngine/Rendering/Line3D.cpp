
#include "stdafx.h"

#include "SpehsEngine/Rendering/Line3D.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Rendering/BatchManager3D.h"
#include "SpehsEngine/Rendering/Vertex3D.h"

#include <glm/vec2.hpp>

#include <GL/glew.h>


namespace se
{
	namespace rendering
	{
		Line3D::Line3D()
		{
			mesh = new Mesh();
			mesh->drawMode = GL_LINES;
		}
		Line3D::~Line3D()
		{
			if (batchManager)
				batchManager->remove(*this);
			delete mesh;
		}

		void Line3D::addPoint(const glm::vec3& _point)
		{
			if (batchManager && mesh->vertexArray.size() > 0)
				batchManager->removeMesh(*mesh);
			if (mesh->vertexArray.size() > 1) mesh->elementArray.push_back((GLushort)mesh->vertexArray.size() - 1);
			mesh->vertexArray.push_back(Vertex3D(_point, glm::vec2(0.0f), glm::vec3(0.0f)));
			mesh->elementArray.push_back((GLushort)mesh->vertexArray.size() - 1);
			if (batchManager)
				batchManager->addMesh(*mesh);
		}
		void Line3D::addPoints(const std::vector<glm::vec3>& _points)
		{
			if (batchManager && mesh->vertexArray.size() > 0)
				batchManager->removeMesh(*mesh);
			for (size_t i = 0; i < _points.size(); i++)
			{
				if (mesh->vertexArray.size() > 1) mesh->elementArray.push_back((GLushort)mesh->vertexArray.size() - 1);
				mesh->vertexArray.push_back(Vertex3D(_points[i], glm::vec2(0.0f), glm::vec3(0.0f)));
				mesh->elementArray.push_back((GLushort)mesh->vertexArray.size() - 1);
			}
			if (batchManager)
				batchManager->addMesh(*mesh);
		}
		void Line3D::updatePoint(const size_t _index, const glm::vec3& _point)
		{
			se_assert(_index < mesh->vertexArray.size());
			mesh->vertexArray[_index].position = _point;
			mesh->needUpdate = true;
		}
		void Line3D::updatePoints(const std::vector<glm::vec3>& _points)
		{
			se_assert(_points.size() == mesh->vertexArray.size());
			for (size_t i = 0; i < _points.size(); i++)
			{
				mesh->vertexArray[i].position = _points[i];
			}
			mesh->needUpdate = true;
		}
		void Line3D::setPointColor(const size_t _index, const Color _color)
		{
			se_assert(_index < mesh->vertexArray.size());
			mesh->vertexArray[_index].color = _color;
			mesh->needUpdate = true;
		}
		void Line3D::clearPoints()
		{
			if (batchManager && mesh->vertexArray.size() > 0)
				batchManager->removeMesh(*mesh);
			mesh->vertexArray.clear();
			mesh->elementArray.clear();
		}

		glm::vec3 Line3D::getPoint(const size_t _index)
		{
			return mesh->vertexArray[_index].position;
		}

		void Line3D::setPosition(const glm::vec3& _newPosition)
		{
			mesh->updatePosition(_newPosition);
		}
		void Line3D::setRotation(const glm::quat& _newRotation)
		{
			mesh->updateRotation(_newRotation);
		}
		void Line3D::setScale(const glm::vec3& _newScale)
		{
			mesh->updateScale(_newScale);
		}
		void Line3D::setColor(const Color _color)
		{
			mesh->setColor(_color);
		}
		void Line3D::setAlpha(const float _alpha)
		{
			mesh->setAlpha(_alpha);
		}
		void Line3D::translate(const glm::vec3& _translation)
		{
			mesh->translate(_translation);
		}
		void Line3D::setBlending(const bool _value)
		{
			mesh->setBlending(_value);
		}
		void Line3D::setDepthTest(const bool _value)
		{
			mesh->setDepthTest(_value);
		}
		void Line3D::setRenderState(const bool _newState)
		{
			mesh->setRenderState(_newState);
		}
		void Line3D::toggleRenderState()
		{
			mesh->toggleRenderState();
		}
		void Line3D::setShaderIndex(const unsigned int _newShaderIndex)
		{
			mesh->setShaderIndex(_newShaderIndex);
		}
		void Line3D::setLineWidth(const float _value)
		{
			mesh->setLineWidth(_value);
		}
		void Line3D::setLineSmoothing(const bool _value)
		{
			mesh->setLineSmoothing(_value);
		}

		glm::vec3 Line3D::getPosition() const
		{
			return mesh->getPosition();
		}
		glm::quat Line3D::getRotation() const
		{
			return mesh->getRotation();
		}
		glm::vec3 Line3D::getScale() const
		{
			return mesh->getScale();
		}
		bool Line3D::getRenderState() const
		{
			return mesh->getRenderState();
		}
		bool Line3D::getBlending() const
		{
			return mesh->getBlending();
		}
		bool Line3D::getDepthTest() const
		{
			return mesh->getDepthTest();
		}
		unsigned int Line3D::getShaderIndex() const
		{
			return mesh->getShaderIndex();
		}
		se::Color Line3D::getColor() const
		{
			return mesh->getColor();
		}
		float Line3D::getAlpha() const
		{
			return mesh->getAlpha();
		}
		float Line3D::getLineWidth() const
		{
			return mesh->getLineWidth();
		}
		bool Line3D::getLineSmoothing() const
		{
			return mesh->getLineSmoothing();
		}
	}
}