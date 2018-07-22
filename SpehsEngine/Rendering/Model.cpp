
#include "stdafx.h"
#include "SpehsEngine/Rendering/Model.h"
#include "SpehsEngine/Rendering/ModelManager.h"
#include "SpehsEngine/Rendering/Mesh.h"
#include "SpehsEngine/Core/Log.h"


namespace se
{
	namespace rendering
	{
		Model::Model()
		{
		}
		Model::Model(BatchManager3D& _batchManager)
		{
			_batchManager.add(*this);
		}
		Model::~Model()
		{
			if (batchManager)
				batchManager->remove(*this);
			for (size_t i = 0; i < meshes.size(); i++)
				delete meshes[i];
		}

		void Model::clearMeshes()
		{
			if (batchManager)
			{
				// Remove just meshes
				for (size_t i = 0; i < meshes.size(); i++)
					batchManager->removeMesh(*meshes[i]);
			}
			for (size_t i = 0; i < meshes.size(); i++)
				delete meshes[i];
			meshes.clear();
		}
		void Model::addMeshes()
		{
			if (batchManager)
			{
				for (size_t i = 0; i < meshes.size(); i++)
					batchManager->addMesh(*meshes[i]);
			}
			else
			{
				se_assert(false);
			}
		}

		void Model::loadModel(const std::string& _filepath)
		{
			if (batchManager)
			{
				clearMeshes();
				batchManager->modelManager.loadModel(_filepath, *this);
				addMeshes();
			}
			else
			{
				se::log::error("Model needs to be added to a BatchManager to load a model.");
			}
		}
		void Model::loadModel(const size_t _hash)
		{
			if (batchManager)
			{
				clearMeshes();
				batchManager->modelManager.loadModel(_hash, *this);
				addMeshes();
			}
			else
			{
				se::log::error("Model needs to be added to a BatchManager to load a model.");
			}
		}

		void Model::setPosition(const glm::vec3& _newPosition)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setPosition(_newPosition);
		}
		void Model::setRotation(const glm::vec3& _newRotation)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setRotation(_newRotation);
		}
		void Model::setScale(const glm::vec3& _newScale)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setScale(_newScale);
		}
		void Model::setColor(const Color _color)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setColor(_color);
		}
		void Model::setAlpha(const float _alpha)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setAlpha(_alpha);
		}
		void Model::translate(const glm::vec3& _translation)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->translate(_translation);
		}
		void Model::setBlending(const bool _value)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setBlending(_value);
		}
		void Model::setDepthTest(const bool _value)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setDepthTest(_value);
		}
		void Model::setRenderState(const bool _newState)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setRenderState(_newState);
		}
		void Model::toggleRenderState()
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->toggleRenderState();
		}
		void Model::setShaderIndex(const unsigned int _newShaderIndex)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setShaderIndex(_newShaderIndex);
		}
		void Model::setBackFaceCulling(const bool _value)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setBackFaceCulling(_value);
		}
		void Model::setTexture(const std::string& _texturePath)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setTexture(_texturePath);
		}
		void Model::setTexture(const size_t _textureID)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setTexture(_textureID);
		}
		void Model::setTexture(TextureData* _textureDataPtr)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setTexture(_textureDataPtr);
		}

		glm::vec3 Model::getPosition() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getPosition();
			else
				return glm::vec3(0.0f);
		}
		glm::vec3 Model::getRotation() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getRotation();
			else
				return glm::vec3(0.0f);
		}
		glm::vec3 Model::getScale() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getScale();
			else
				return glm::vec3(0.0f);
		}
		bool Model::getBackFaceCulling() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getBackFaceCulling();
			else
				return false;
		}
		bool Model::getRenderState() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getRenderState();
			else
				return false;
		}
		bool Model::getBlending() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getBlending();
			else
				return false;
		}
		bool Model::getDepthTest() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getDepthTest();
			else
				return false;
		}
		unsigned int Model::getShaderIndex() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getShaderIndex();
			else
				return 0;
		}
		se::Color Model::getColor() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getColor();
			else
				return Color();
		}
		float Model::getAlpha() const
		{
			if (meshes.size() > 0)
				return meshes[0]->getAlpha();
			else
				return 0.0f;
		}
	}
}
