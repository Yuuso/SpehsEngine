
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
		class Model;
		class Plane3D;
		class Line3D;
		class Mesh;
		class MeshBatch;
		class Window;
		class ShaderManager;
		class TextureManager;
		class ModelManager;

		class BatchManager3D
		{
			friend class Mesh;
			friend class Model;
			friend class Line3D;

		public:
			BatchManager3D(Window& _window, ModelManager& _modelManager, ShaderManager& _shaderManager, Camera3D& _camera, const std::string& _name = "unnamed 3d batch manager");
			~BatchManager3D();

			void add(Model& _model);
			void remove(Model& _model);

			void add(Plane3D& _plane);
			void remove(Plane3D& _plane);

			void add(Line3D& _line);
			void remove(Line3D& _line);

			void render();

			ShaderManager& shaderManager;
			TextureManager& textureManager;
			ModelManager& modelManager;
			Window& window;
			Camera3D& camera3D;
			const std::string name;

		protected:
			void addMesh(Mesh& _mesh);
			void removeMesh(Mesh& _mesh);
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
