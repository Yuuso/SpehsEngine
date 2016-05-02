#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

#include "Depth.h"



namespace spehs
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
	class Vertex;
	class Position;

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
		Mesh* createMesh(const std::string &_filePath);

		Polygon* createPolygon(const int &_shapeID, const PlaneDepth &_planeDepth = 0, const float &_width = 1.0f, const float &_height = 1.0f);
		Polygon* createPolygon(std::vector<spehs::Vertex> _vertexData, const PlaneDepth &_planeDepth, const float &_width = 1.0f, const float &_height = 1.0f);
		Polygon* createPolygon(std::vector<spehs::Vertex> _vertexData, const float &_width = 1.0f, const float &_height = 1.0f);
		Polygon* createPolygon(std::vector<spehs::Position> _cuspData, const PlaneDepth &_planeDepth, const float &_width = 1.0f, const float &_height = 1.0f);

		Line* createLine(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth = 0);
		Line* createLine(const glm::vec3 &_startPoint, const glm::vec3 &_endPoint);

		Point* createPoint(const PlaneDepth &_planeDepth = 0);

		void render();
		void clearBatches();

		Camera2D* getCamera2D(){ return camera2D; }
		Camera3D* getCamera3D(){ return camera3D; }

	protected:
		std::vector<PrimitiveBatch*> primitiveBatches;
		std::vector<MeshBatch*> meshBatches;

		std::vector<Primitive*> primitives;
		std::vector<Mesh*> meshes;

		Camera2D* camera2D;
		Camera3D* camera3D;
	};

	/*
	Active BatchManager needs to be set when using some parts of the engine!
	*/
	void setActiveBatchManager(BatchManager* _active);
	void resetActiveBatchManager();
	BatchManager* getActiveBatchManager();
}
