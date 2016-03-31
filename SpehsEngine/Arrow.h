#pragma once

#include <glm\vec2.hpp>

namespace spehs{ class Line; class Polygon; }

namespace spehs
{
	class Arrow
	{
	public:
		Arrow();
		Arrow(const glm::vec2& _startPoint, const glm::vec2& _endPoint);
		~Arrow();

		void setPosition(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint);
		void setLineThickness(const float &_thickness);
		void setArrowPointerSize(const float &_sizeX, const float &_sizeY);
		void setArrowPointerSize(const glm::vec2 &_ref);
		void setArrowColor(const int &_r, const int &_b, const int &_g, const int &_a);
		void setArrowColor(const glm::vec4 &_color);
		void setRenderState(const bool _value);
		void cameraState(const bool _value);

	private:
		void createArrow(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint);

		Line* linePart = nullptr;
		Polygon* polygonPart = nullptr;
		float arrowSizeX = 10.0f;
		float arrowSizeY = 10.0f;
	};
}