#pragma once


#include <vector>
#include <stack>
#include <string>

#include "SpehsEngine/Rendering/BatchRenderResults.h"
#include "SpehsEngine/Rendering/Depth.h"
#include "SpehsEngine/Core/Vector.h"

namespace spehs
{
	class Primitive;
	class Polygon;
	class Line;
	class Point;
	class Text;

	class ShaderManager;
	class TextureManager;
	class Batch;
	class Camera2D;
	class Vertex;
	class Position;
	class Window;

	/*
		Class that handles the management and rendering of batches and batchobjects.
		BatchManager creates and deletes all batchobjects (primitives and texts).
		Supplied window and camera references must be valid throughout the batch manager's lifetime.
	*/
	class BatchManager
	{
	public:
		BatchManager(Window& _window, Camera2D& _camera, TextureManager& _textureManger, ShaderManager& _shaderManager, const std::string& _name = "unnamed batch manager");
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

		void render(BatchRenderResults* results = nullptr);
		void clearBatches();

		TextureManager& textureManager;
		ShaderManager& shaderManager;
		Window& window;
		Camera2D& camera2D;
		const std::string name;

	protected:


		std::vector<Batch*> batches;
		std::vector<Primitive*> primitives;
		std::vector<Text*> texts;
	};
}