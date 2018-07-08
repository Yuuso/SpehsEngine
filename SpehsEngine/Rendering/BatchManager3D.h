
#pragma once

#include "SpehsEngine/Rendering/Vertex3D.h"

#include <string>
#include <vector>


namespace spehs
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

		Mesh* createMesh();
		Mesh* createMesh(const std::string& _filepath);

		void render();
		void clearBatches();

		ShaderManager& shaderManager;
		TextureManager& textureManager;
		ModelManager& modelManager;
		Window& window;
		Camera3D& camera3D;
		const std::string name;

	protected:
		std::vector<MeshBatch*> meshBatches;
		std::vector<Mesh*> meshes;
	};
}
