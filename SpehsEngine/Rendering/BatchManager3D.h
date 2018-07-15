
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"

#include <string>
#include <vector>
#include <utility>


namespace se
{
	namespace rendering
	{
		class Camera3D;
		class Mesh;
		class MeshBatch;
		class Window;
		class ShaderManager;
		class TextureManager;
		class ModelManager;

		class BatchManager3D
		{
			friend class Mesh;

		public:
			BatchManager3D(Window& _window, ModelManager& _modelManager, ShaderManager& _shaderManager, Camera3D& _camera, const std::string& _name = "unnamed 3d batch manager");
			~BatchManager3D();

			void addMesh(Mesh& mesh);
			void removeMesh(Mesh& mesh);
			void updateMesh(Mesh& mesh);

			void render();

			ShaderManager& shaderManager;
			TextureManager& textureManager;
			ModelManager& modelManager;
			Window& window;
			Camera3D& camera3D;
			const std::string name;

		private:
			struct MeshObject
			{
				MeshObject(Mesh& _mesh) : mesh(&_mesh) {}
				Mesh* mesh;
				MeshBatch* batch = nullptr;
				std::pair<size_t, size_t> indexInBatch;
				std::pair<size_t, size_t> sizeInBatch;
			};
			void batch(MeshObject& _object, MeshBatch* _batch);
			void unbatch(MeshObject& _object);
			void updateMeshVertices(MeshObject& _object);
			std::vector<MeshObject> meshes;

			std::vector<MeshBatch*> batches;
		};
	}
}
