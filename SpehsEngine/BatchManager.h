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
	class Text;

	class Batch;
	class Camera2D;
	class Vertex;
	class Position;

	/*
	Class that handles the management and rendering of batches and batchobjects.
	BatchManager creates and deletes all batchobjects (primitives and meshes).
	*/
	class BatchManager
	{
	public:
		BatchManager(Camera2D* _camera);
		~BatchManager();

		Polygon* createPolygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height);
		Polygon* createPolygon(std::vector<spehs::Vertex> _vertexData, const PlaneDepth &_planeDepth, const float &_width, const float &_height);
		Polygon* createPolygon(std::vector<spehs::Vertex> _vertexData, const float &_width, const float &_height);
		Polygon* createPolygon(std::vector<spehs::Position> _cuspData, const PlaneDepth &_planeDepth, const float &_width, const float &_height);

		Line* createLine(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth);
		Line* createLine(const glm::vec3 &_startPoint, const glm::vec3 &_endPoint);

		Point* createPoint(const PlaneDepth &_planeDepth);

		Text* createText(const PlaneDepth &_planeDepth);

		void render();
		void clearBatches();

		Camera2D* getCamera2D(){ return camera2D; }

	protected:
		std::vector<Batch*> batches;

		std::vector<Primitive*> primitives;
		std::vector<Text*> texts;

		Camera2D* camera2D;
	};

	/*
	Active BatchManager needs to be set when using some parts of the engine!
	*/
	void setActiveBatchManager(BatchManager* _active);
	void resetActiveBatchManager();
	BatchManager* getActiveBatchManager();
}
