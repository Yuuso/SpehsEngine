
#pragma once

#include "SpehsEngine\Rendering\Vertex3D.h"

#include <vector>
#include <string>
#include <unordered_map>


struct aiNode;
struct aiScene;
struct aiMesh;
namespace se
{
	namespace rendering
	{
		class Model;
		class ModelManager
		{
			// Minimal classes for storage
			struct Mesh
			{
				std::vector<Vertex3D> vertices;
				std::vector<uint16_t> indices;
			};
			struct Model
			{
				std::vector<ModelManager::Mesh> meshes;
			};

		public:
			ModelManager();
			~ModelManager();

			void clearAllModelData();

			void loadModel(const std::string& _filepath, se::rendering::Model& _model);
			void loadModel(const size_t _hash, se::rendering::Model& _model);
			size_t preloadModel(const std::string& _filepath);

			void removeModelData(const std::string& _filepath);
			void removeModelData(const size_t& _hash);

		private:
			void processNode(ModelManager::Model* _model, aiNode* _node, const aiScene* _scene, glm::mat4& _transformation);
			void addMesh(ModelManager::Model* _model, aiMesh* _mesh, const aiScene* _scene, glm::mat4& _transformation);

			std::unordered_map<size_t, ModelManager::Model*> modelDataMap;
		};
	}
}
