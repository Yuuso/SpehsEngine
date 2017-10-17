#pragma once


#include <vector>
#include <stack>
#include <string>

#include "Depth.h"
#include "Vector.h"

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
	//Batch section management
	private:
		std::stack<BatchManager*> previousSections;///Stack of batches that were active during each call to beginSection(). Stack resolves as endSection() is being called one by one
	public:
		void beginSection();
		void endSection();

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

	/*
		Active BatchManager needs to be set when using some parts of the engine!
	*/
	void setActiveBatchManager(BatchManager* _active);
	void resetActiveBatchManager();
	BatchManager* getActiveBatchManager();
	/*
		RAII style batch section
	*/
	class BatchSection
	{
	public:
		BatchSection(BatchManager& _batchManager)
			: batchManager(_batchManager)
		{
			batchManager.beginSection();
		}
		~BatchSection()
		{
			batchManager.endSection();
		}
	private:
		BatchManager& batchManager;
	};
}