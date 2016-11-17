#pragma once

#include "CommonColor.h"
#include "Depth.h"

#include <glm\vec2.hpp>

namespace spehs{ class Line; class Polygon; }

namespace spehs
{
	class Arrow
	{
	public:
		Arrow(const int16_t planeDepth = 0);
		Arrow(const glm::vec2& _startPoint, const glm::vec2& _endPoint);
		~Arrow();

		void setPosition(const glm::vec2& _startPoint, const glm::vec2& _endPoint);
		void setLineThickness(const float _thickness);
		void setArrowPointerSize(const float _pointerWidth, const float _pointerHeight);
		void setArrowPointerSize(const glm::vec2& _ref);
		void setArrowColor(const int _r, const int _b, const int _g, const int _a);
		void setArrowColor(const glm::vec4& _color);
		void setArrowColor(const CommonColor& _color);
		void setRenderState(const bool _value);
		void setDepth(const PlaneDepth _value);
		void setCameraMatrixState(const bool _value);

	private:
		Line* linePart;
		Polygon* polygonPart;
		float pointerWidth;
		float pointerHeight;
		float length;//Length from start to end
	};
}