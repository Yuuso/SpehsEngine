#include "stdafx.h"

#include "SpehsEngine/Rendering/ModelManager.h"
#include "SpehsEngine/Rendering/Model.h"
#include "SpehsEngine/Rendering/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


namespace se
{
	namespace rendering
	{
		ModelManager::ModelManager()
		{

		}
		ModelManager::~ModelManager()
		{
			clearAllModelData();
		}

		void ModelManager::clearAllModelData()
		{
			for (auto &it : modelDataMap)
				delete it.second;
			modelDataMap.clear();
		}

		void ModelManager::processNode(ModelManager::Model* _model, aiNode* _node, const aiScene* _scene, aiMatrix4x4 _transform)
		{
			_transform *= _node->mTransformation;
			aiVector3D position;
			aiQuaternion rotation;
			aiVector3D scale;
			_transform.Decompose(scale, rotation, position);
			for (unsigned int i = 0; i < _node->mNumMeshes; i++)
			{
				addMesh(_model, _scene->mMeshes[_node->mMeshes[i]]);
				_model->meshes.back().position = glm::vec3(position.x, position.y, position.z);
				_model->meshes.back().rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
				_model->meshes.back().scale = glm::vec3(scale.x, scale.y, scale.z);
			}

			for (unsigned int i = 0; i < _node->mNumChildren; i++)
				processNode(_model, _node->mChildren[i], _scene, _transform);
		}
		void ModelManager::addMesh(ModelManager::Model* _model, aiMesh* _mesh)
		{
			_model->meshes.push_back(ModelManager::Mesh());
			ModelManager::Mesh& mesh = _model->meshes.back();
			mesh.name = std::string(_mesh->mName.C_Str());
			// Note! Node names are not used! (should they be?)

			Vertex3D vertex;
			// Vertices
			for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
			{
				vertex.position = glm::vec3(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);
				vertex.normal = glm::vec3(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
				if (_mesh->mTextureCoords[0])
				{
					vertex.uv.x = _mesh->mTextureCoords[0][i].x;
					vertex.uv.y = _mesh->mTextureCoords[0][i].y;
				}
				mesh.vertices.push_back(vertex);
			}

			// Indices
			for (unsigned int i = 0; i < _mesh->mNumFaces; i++)
			{
				aiFace& face = _mesh->mFaces[i];
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					mesh.indices.push_back(face.mIndices[j]);
			}

			// Materials
			if(_mesh->mMaterialIndex >= 0)
			{
				// TODO?
			}
		}

		void ModelManager::loadModel(const std::string& _filepath, se::rendering::Model& _model)
		{
			const size_t hash = std::hash<std::string>()(_filepath);
			std::unordered_map<size_t, Model*>::iterator it = modelDataMap.find(hash);
			if (it == modelDataMap.end())
				preloadModel(_filepath);
			loadModel(hash, _model);
		}
		void ModelManager::loadModel(const size_t _hash, se::rendering::Model& _model)
		{
			std::unordered_map<size_t, Model*>::iterator it = modelDataMap.find(_hash);
			if (it == modelDataMap.end())
			{
				log::error("Couldn't find model data: " + std::to_string(_hash));
			}
			else
			{
				ModelManager::Model* model = it->second;
				for (size_t i = 0; i < model->meshes.size(); i++)
				{
					if (model->meshes[i].vertices.size() == 0)
						continue;
					_model.meshes.push_back(std::unique_ptr<se::rendering::Mesh>(new se::rendering::Mesh()));
					_model.meshes[i]->name = model->meshes[i].name;
					_model.meshes[i]->localPosition = model->meshes[i].position;
					_model.meshes[i]->localRotation = model->meshes[i].rotation;
					_model.meshes[i]->localScale = model->meshes[i].scale;
					_model.meshes[i]->vertexArray = model->meshes[i].vertices;
					_model.meshes[i]->elementArray = model->meshes[i].indices;
				}
			}
		}
		size_t ModelManager::preloadModel(const std::string& _filepath)
		{
			const size_t hash = std::hash<std::string>()(_filepath);
			std::unordered_map<size_t, Model*>::iterator it = modelDataMap.find(hash);
			if (it != modelDataMap.end())
			{
				log::warning("Loading an already existing model: " + _filepath);
				return hash;
			}
			it = modelDataMap.insert(std::pair<size_t, ModelManager::Model*>(hash, new ModelManager::Model())).first;

			log::info("Loading model: " + _filepath, log::TextColor::DARKCYAN);
			Assimp::Importer importer;
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, UINT16_MAX);
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, UINT16_MAX / 3);
			const aiScene* scene = importer.ReadFile(_filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_SplitLargeMeshes);
			if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				log::error("Error loading model: " + _filepath + " | Assimp error: " + importer.GetErrorString());
				return 0;
			}

			processNode(it->second, scene->mRootNode, scene, aiMatrix4x4());
			return hash;
		}

		void ModelManager::removeModelData(const std::string& _filepath)
		{
			const size_t hash = std::hash<std::string>()(_filepath);
			auto it = modelDataMap.find(hash);
			if (it == modelDataMap.end())
			{
				log::error("Couldn't find model data: " + _filepath);
				return;
			}
			delete it->second;
			modelDataMap.erase(hash);
		}

		void ModelManager::removeModelData(const size_t& _hash)
		{
			auto it = modelDataMap.find(_hash);
			if (it == modelDataMap.end())
			{
				log::error("Couldn't find model data: " + std::to_string(_hash));
				return;
			}
			delete it->second;
			modelDataMap.erase(_hash);
		}
	}
}