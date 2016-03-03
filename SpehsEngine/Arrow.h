#pragma once

#include "LineBatch.h"
#include "PolygonBatch.h"

#include <glm\vec2.hpp>


namespace spehs
{
	class Arrow
	{
	public:
		Arrow();
		Arrow(glm::vec2& _startPoint, glm::vec2& _endPoint);
		~Arrow();

		void setPosition(glm::vec2& _startPoint, glm::vec2& _endPoint);
		void setLineThickness(float _thickness);
		void setArrowPointerSize(float _sizeX, float _sizeY);
		void setArrowPointerSize(glm::vec2& _ref);
		void setArrowColor(int _r, int _b, int _g, int _a);
		void setArrowColor(glm::vec4& _color);
		void render();

	private:
		void createArrow(glm::vec2& _startPoint, glm::vec2& _endPoint);

		LineBatch* linePart = nullptr;
		PolygonBatch* polygonPart = nullptr;
		float arrowSizeX = 10.0f;
		float arrowSizeY = 10.0f;
	};
}