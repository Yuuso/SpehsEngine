#pragma once


#include <vector>
#include <stack>
#include <string>

#include "SpehsEngine/Rendering/BatchRenderResults.h"
#include "SpehsEngine/Rendering/Depth.h"
#include "SpehsEngine/Core/Vector.h"

namespace se
{
	class Primitive;
	class Polygon;
	class Line;
	class Point;
	class Text;

	class ShaderManager;
	class TextureManager;
	class FontManager;
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
		BatchManager(Window& _window, ShaderManager& _shaderManager, Camera2D& _camera, const std::string& _name = "unnamed batch manager");
		~BatchManager();

		Polygon* createPolygon(const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height);
		Polygon* createPolygon(std::vector<se::Vertex> _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height);
		Polygon* createPolygon(std::vector<se::Vertex> _vertexData, const float _width, const float _height);
		Polygon* createPolygon(std::vector<se::vec2> _cuspData, const PlaneDepth _planeDepth, const float _width, const float _height);

		Line* createLine(const se::vec2& _startPoint, const se::vec2& _endPoint, const PlaneDepth _planeDepth);
		Line* createLine(const PlaneDepth _planeDepth);

		Point* createPoint(const PlaneDepth _planeDepth);

		Text* createText(const PlaneDepth _planeDepth = DEPTH_MAX);
		Text* createText(const std::string& _string, const PlaneDepth _planeDepth = DEPTH_MAX);

		void render(BatchRenderResults* results = nullptr);
		void clearBatches();

		ShaderManager& shaderManager;
		TextureManager& textureManager;
		FontManager& fontManager;
		Window& window;
		Camera2D& camera2D;
		const std::string name;

	protected:


		std::vector<Batch*> batches;
		std::vector<Primitive*> primitives;
		std::vector<Text*> texts;
	};
}