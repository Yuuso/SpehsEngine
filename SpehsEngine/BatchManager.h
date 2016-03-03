#pragma once

#include <vector>


namespace SpehsEngine
{
	class Primitive;
	class Polygon;
	class Line;
	class Point;
	class Mesh;
	class Camera2D;
	class Camera3D;
	class PrimitiveBatch;
	class MeshBatch;

	/*
	Class that handles the management and rendering of batches and batchobjects.
	BatchManager creates and deletes all batchobjects (primitives and meshes).
	*/
	class BatchManager
	{
		friend class Primitive;
		friend class Polygon;
		friend class Line;
		friend class Point;
		friend class Mesh;

	public:
		BatchManager(Camera2D* _camera);
		BatchManager(Camera3D* _camera);
		~BatchManager();

		Mesh* createMesh();
		Polygon* createPolygon();
		Line* createLine();
		Point* createPoint();

		void render();
		void clearBatches();

	protected:
		std::vector<PrimitiveBatch*> primitiveBatches;
		std::vector<MeshBatch*> meshBatches;

		std::vector<Primitive*> primitives;
		std::vector<Mesh*> meshes;

		Camera2D* camera2D;
		Camera3D* camera3D;
	};
}
