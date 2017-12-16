#pragma once


#include <vector>
#include <stack>
#include <string>

#include "SpehsEngine/Rendering/Depth.h"
#include "SpehsEngine/Core/Vector.h"

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
	BatchManager creates and deletes all batchobjects (primitives and texts).
	*/
	class BatchManager
	{
	public:
		BatchManager(Camera2D* _camera, const std::string _name = "unnamed batch manager");
		~BatchManager();

		Polygon* createPolygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height);
		Polygon* createPolygon(std::vector<spehs::Vertex> _vertexData, const PlaneDepth &_planeDepth, const float &_width, const float &_height);
		Polygon* createPolygon(std::vector<spehs::Vertex> _vertexData, const float &_width, const float &_height);
		Polygon* createPolygon(std::vector<spehs::vec2> _cuspData, const PlaneDepth &_planeDepth, const float &_width, const float &_height);

		Line* createLine(const spehs::vec2 &_startPoint, const spehs::vec2 &_endPoint, const PlaneDepth &_planeDepth);
		Line* createLine(const PlaneDepth &_planeDepth);

		Point* createPoint(const PlaneDepth &_planeDepth);

		Text* createText(const PlaneDepth &_planeDepth = DEPTH_MAX);
		Text* createText(const std::string &_string, const PlaneDepth &_planeDepth = DEPTH_MAX);

		void render();
		void clearBatches();

		Camera2D* getCamera2D(){ return camera2D; }


	protected:
		const std::string name;
		std::vector<Batch*> batches;

		std::vector<Primitive*> primitives;
		std::vector<Text*> texts;

		Camera2D* camera2D;
	};
}