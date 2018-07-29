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

		inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4* from)
		{
			glm::mat4 to;
			to[0][0] = from->a1; to[0][1] = from->b1;  to[0][2] = from->c1; to[0][3] = from->d1;
			to[1][0] = from->a2; to[1][1] = from->b2;  to[1][2] = from->c2; to[1][3] = from->d2;
			to[2][0] = from->a3; to[2][1] = from->b3;  to[2][2] = from->c3; to[2][3] = from->d3;
			to[3][0] = from->a4; to[3][1] = from->b4;  to[3][2] = from->c4; to[3][3] = from->d4;
			return to;
		}

		void ModelManager::processNode(ModelManager::Model* _model, aiNode* _node, const aiScene* _scene, glm::mat4& _transformation)
		{
			_transformation *= aiMatrix4x4ToGlm(&_node->mTransformation);
			for (unsigned int i = 0; i < _node->mNumMeshes; i++)
				addMesh(_model, _scene->mMeshes[_node->mMeshes[i]], _scene, _transformation);

			for (unsigned int i = 0; i < _node->mNumChildren; i++)
				processNode(_model, _node->mChildren[i], _scene, _transformation);
		}
		void ModelManager::addMesh(ModelManager::Model* _model, aiMesh* _mesh, const aiScene* _scene, glm::mat4& _transformation)
		{
			_model->meshes.push_back(ModelManager::Mesh());
			ModelManager::Mesh& mesh = _model->meshes.back();

			Vertex3D vertex;
			glm::vec4 transformVertex;
			glm::mat4 normalMatrix = glm::mat4(glm::inverse(glm::transpose(glm::mat3(_transformation))));
			// Vertices
			for (unsigned int i = 0; i < _mesh->mNumVertices; i++)
			{
				transformVertex = _transformation * glm::vec4(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z, 1.0f);
				vertex.position.x = transformVertex.x;
				vertex.position.y = transformVertex.y;
				vertex.position.z = transformVertex.z;
				transformVertex = normalMatrix * glm::vec4(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z, 1.0f);
				vertex.normal.x = transformVertex.x;
				vertex.normal.y = transformVertex.y;
				vertex.normal.z = transformVertex.z;
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
				// TODO
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
					_model.meshes.push_back(new se::rendering::Mesh());
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

			Assimp::Importer importer;
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_VERTEX_LIMIT, UINT16_MAX);
			importer.SetPropertyInteger(AI_CONFIG_PP_SLM_TRIANGLE_LIMIT, UINT16_MAX / 3);
			const aiScene* scene = importer.ReadFile(_filepath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_SplitLargeMeshes);
			if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			{
				log::error("Error loading model: " + _filepath + " | Assimp error: " + importer.GetErrorString());
				return 0;
			}

			processNode(it->second, scene->mRootNode, scene, glm::mat4(1.0f));
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