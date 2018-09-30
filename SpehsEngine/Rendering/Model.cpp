
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
		Model::~Model()
		{
			for (size_t b = 0; b < batchManagers.size(); /*b++*/)
				batchManagers[b]->remove(*this);
		}

		void Model::loadModel(ModelManager& _modelManager, const std::string& _filepath)
		{
			for (size_t b = 0; b < batchManagers.size(); b++)
				for (size_t m = 0; m < meshes.size(); m++)
					batchManagers[b]->removeMesh(*meshes[m]);

			_modelManager.loadModel(_filepath, *this);

			for (size_t b = 0; b < batchManagers.size(); b++)
			{
				for (size_t m = 0; m < meshes.size(); m++)
				{
					updateMeshTransform(*meshes[m]);
					batchManagers[b]->addMesh(*meshes[m]);
				}
			}
		}
		void Model::loadModel(ModelManager& _modelManager, const size_t _hash)
		{
			for (size_t b = 0; b < batchManagers.size(); b++)
				for (size_t m = 0; m < meshes.size(); m++)
					batchManagers[b]->removeMesh(*meshes[m]);

			_modelManager.loadModel(_hash, *this);

			for (size_t b = 0; b < batchManagers.size(); b++)
			{
				for (size_t m = 0; m < meshes.size(); m++)
				{
					updateMeshTransform(*meshes[m]);
					batchManagers[b]->addMesh(*meshes[m]);
				}
			}
		}

		void Model::addMesh(const Mesh& _mesh)
		{
			meshes.push_back(std::unique_ptr<se::rendering::Mesh>(new Mesh(_mesh)));
			for (size_t b = 0; b < batchManagers.size(); b++)
			{
				updateMeshTransform(*meshes.back());
				batchManagers[b]->addMesh(*meshes.back());
			}
		}
		void Model::insertMesh(const size_t _position, const Mesh& _mesh)
		{
			meshes.insert(meshes.begin() + _position, std::unique_ptr<se::rendering::Mesh>(new Mesh(_mesh)));
			for (size_t b = 0; b < batchManagers.size(); b++)
			{
				updateMeshTransform(*meshes.back());
				batchManagers[b]->addMesh(*meshes[_position]);
			}
		}
		void Model::replaceMesh(const size_t _position, const Mesh& _mesh)
		{
			se_assert(meshes.size() > _position);
			for (size_t b = 0; b < batchManagers.size(); b++)
				batchManagers[b]->removeMesh(*meshes[_position]);
			meshes[_position] = std::unique_ptr<se::rendering::Mesh>(new Mesh(_mesh));
			for (size_t b = 0; b < batchManagers.size(); b++)
			{
				updateMeshTransform(*meshes.back());
				batchManagers[b]->addMesh(*meshes[_position]);
			}
		}
		std::vector<Mesh*> Model::findMeshes(const std::string& _name)
		{
			se_assert(meshes.size() > 0);
			std::vector<Mesh*> result;
			for (size_t m = 0; m < meshes.size(); m++)
				if (meshes[m]->name == _name)
					result.push_back(meshes[m].get());
			if (result.size() == 0)
				se::log::error("Model::findMesh: Meshes with name \"" + _name + "\" not found!");
			return result;
		}
		Mesh* Model::getMesh(size_t _index)
		{
			se_assert(meshes.size() > _index);
			return meshes[_index].get();
		}
		void Model::removeMeshes(const std::string& _name)
		{
			se_assert(meshes.size() > 0);
#ifdef _DEBUG
			int removed = 0;
#endif
			for (size_t m = 0; m < meshes.size(); /*m++*/)
			{
				if (meshes[m]->name == _name)
				{
					for (size_t b = 0; b < batchManagers.size(); b++)
						batchManagers[b]->removeMesh(*meshes[m]);
					meshes.erase(meshes.begin() + m);
#ifdef _DEBUG
					removed++;
#endif
				}
				else
					m++;
			}
#ifdef _DEBUG
			if (removed == 0)
				se::log::error("Model::removeMeshes: Meshes with name \"" + _name + "\" not found!");
#endif
		}
		void Model::removeMesh(const size_t _index)
		{
			se_assert(meshes.size() > _index);
			for (size_t b = 0; b < batchManagers.size(); b++)
				batchManagers[b]->removeMesh(*meshes[_index]);
			meshes.erase(meshes.begin() + _index);
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
		void Model::setTexture(TextureManager& _textureManager, const std::string& _texturePath, const size_t _index)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setTexture(_textureManager, _texturePath, _index);
		}
		void Model::setTexture(TextureManager& _textureManager, const size_t _textureID, const size_t _index)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setTexture(_textureManager, _textureID, _index);
		}
		void Model::setTexture(TextureData* _textureDataPtr, const size_t _index)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->setTexture(_textureDataPtr, _index);
		}


		void Model::setPosition(const glm::vec3& _newPosition)
		{
			position = _newPosition;
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->updatePosition(position);
		}
		void Model::setRotation(const glm::quat& _newRotation)
		{
			rotation = _newRotation;
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->updateRotation(rotation);
		}
		void Model::setScale(const glm::vec3& _newScale)
		{
			scale = _newScale;
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->updateScale(scale);
		}
		void Model::translate(const glm::vec3& _translation)
		{
			position += _translation;
			for (size_t i = 0; i < meshes.size(); i++)
				meshes[i]->updatePosition(position);
		}

		void Model::updateMeshTransform(Mesh& _mesh)
		{
			_mesh.updatePosition(position);
			_mesh.updateRotation(rotation);
			_mesh.updateScale(scale);
		}
	}
}
