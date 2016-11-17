#define PI 3.14159265358979323846264338

#include "Arrow.h"
#include "Line.h"
#include "Polygon.h"
#include "Geometry.h"
#include "Exceptions.h"
#include "BatchManager.h"


namespace spehs
{
	static std::vector<glm::vec2> pointerVertices = { glm::vec2(-0.5f, 0.5f), glm::vec2(-0.5f, -0.5f), glm::vec2(0.5f, 0.0f) };
	Arrow::Arrow(const int16_t planeDepth) : linePart(nullptr), polygonPart(nullptr), pointerWidth(1.0f), pointerHeight(1.0f), length(0.0f)
	{
		//Create needed primitive components
		polygonPart = getActiveBatchManager()->createPolygon(pointerVertices, planeDepth, pointerWidth, pointerHeight);
		polygonPart->setCameraMatrixState(false);
		linePart = getActiveBatchManager()->createLine(glm::vec2(0), glm::vec2(0), planeDepth);
		linePart->setCameraMatrixState(false);
		linePart->setRenderState(false);//Length == 0
	}
	Arrow::Arrow(const glm::vec2& _startPoint, const glm::vec2& _endPoint) : Arrow()
	{
		setPosition(_startPoint, _endPoint);
	}
	Arrow::~Arrow()
	{
		linePart->destroy();
		polygonPart->destroy();
	}


	void Arrow::setPosition(const glm::vec2& _startPoint, const glm::vec2& _endPoint)
	{
		float angle = atan2(_endPoint.y - _startPoint.y, _endPoint.x - _startPoint.x);
		glm::vec2 endOfLine(_endPoint - _startPoint);

		//Arrow length changed
		length = magnitude(endOfLine);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(polygonPart->getRenderState());

		//End of line position
		endOfLine -= glm::vec2(cos(angle) * pointerHeight, sin(angle) * pointerHeight);

		//Primitive positioning
		linePart->setPoints(_startPoint, endOfLine);
		polygonPart->setPosition((endOfLine.x + _endPoint.x) * 0.5f, (endOfLine.y + _endPoint.y) * 0.5f);

		//Polygon rotation
		polygonPart->setRotation(angle);
	}


	void Arrow::setLineThickness(const float _thickness)
	{
		linePart->setLineWidth(_thickness);
	}


	void Arrow::setArrowPointerSize(const float _pointerWidth, const float _pointerHeight)
	{
		pointerWidth = _pointerWidth;
		pointerHeight = _pointerHeight;
		polygonPart->resize(pointerHeight, pointerWidth);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(polygonPart->getRenderState());
	}
	void Arrow::setArrowPointerSize(const glm::vec2 &_ref)
	{
		pointerWidth = _ref.x;
		pointerHeight = _ref.y;
		polygonPart->resize(pointerHeight, pointerWidth);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(polygonPart->getRenderState());
	}


	void Arrow::setArrowColor(const int _r, const int _b, const int _g, const int _a)
	{
		linePart->setColor(_r, _b, _g, _a);
		polygonPart->setColor(_r, _b, _g, _a);
	}


	void Arrow::setArrowColor(const glm::vec4 &_color)
	{
		linePart->setColor(_color);
		polygonPart->setColor(_color);
	}


	void Arrow::setArrowColor(const CommonColor &_color)
	{
		linePart->setColor(_color);
		polygonPart->setColor(_color);
	}


	void Arrow::setRenderState(const bool _value)
	{
		polygonPart->setRenderState(_value);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(polygonPart->getRenderState());
	}


	void Arrow::setDepth(const PlaneDepth _value)
	{
		linePart->setPlaneDepth(_value);
		polygonPart->setPlaneDepth(_value);
	}


	void Arrow::setCameraMatrixState(const bool _value)
	{
		linePart->setCameraMatrixState(_value);
		polygonPart->setCameraMatrixState(_value);
	}

}