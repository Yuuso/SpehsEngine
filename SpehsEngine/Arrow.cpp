#define PI 3.14159265358979323846264338

#include "Arrow.h"
#include "Line.h"
#include "Polygon.h"
#include "Exceptions.h"
#include "BatchManager.h"


namespace spehs
{
	Arrow::Arrow()
	{//Default constructor

	}
	Arrow::Arrow(const glm::vec2& _startPoint, const glm::vec2& _endPoint)
	{
		createArrow(_startPoint, _endPoint);
	}
	Arrow::~Arrow()
	{
		if (linePart)
			linePart->destroy();
		if (polygonPart)
			polygonPart->destroy();
	}


	void Arrow::setPosition(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint)
	{
		//There's probably a better way of doing things...
		createArrow(_startPoint, _endPoint);
	}


	void Arrow::setLineThickness(const float &_thickness)
	{
		if (linePart == nullptr)
		{
			return;
		}
		linePart->setLineWidth(_thickness);
	}


	void Arrow::setArrowPointerSize(const float &_sizeX, const float &_sizeY)
	{
		arrowSizeX = _sizeX;
		arrowSizeY = _sizeY;
	}
	void Arrow::setArrowPointerSize(const glm::vec2 &_ref)
	{
		arrowSizeX = _ref.x;
		arrowSizeY = _ref.y;
	}


	void Arrow::setArrowColor(const int &_r, const int &_b, const int &_g, const int &_a)
	{
		if (linePart)
			linePart->setColor(_r, _b, _g, _a);
		if (polygonPart)
			polygonPart->setColor(_r, _b, _g, _a);
	}


	void Arrow::setArrowColor(const glm::vec4 &_color)
	{
		if (linePart)
			linePart->setColor(_color);
		if (polygonPart)
			polygonPart->setColor(_color);
	}


	void Arrow::setRenderState(const bool _value)
	{
		if (linePart)
			linePart->setRenderState(_value);
		if (polygonPart)
			polygonPart->setRenderState(_value);
	}


	void Arrow::cameraState(const bool _value)
	{
		if (linePart)
			linePart->setCameraMatrixState(_value);
		if (polygonPart)
			polygonPart->setCameraMatrixState(_value);
	}


	//Private:
	void Arrow::createArrow(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint)
	{
		if (_startPoint == _endPoint)
			return;

		if (linePart != nullptr)
			linePart->destroy();
		if (polygonPart != nullptr)
			polygonPart->destroy();

		linePart = getActiveBatchManager()->createLine(_startPoint, _endPoint);
		linePart->setCameraMatrixState(false);

		float _angle = -atan2(_endPoint.x - _startPoint.x, _endPoint.y - _startPoint.y);

		polygonPart = getActiveBatchManager()->createPolygon(3, arrowSizeX, arrowSizeY);
		polygonPart->setPosition(_endPoint.x + 0.5f*arrowSizeY*cos(_angle - 0.5f * PI), _endPoint.y + 0.5f*arrowSizeY*sin(_angle - 0.5f * PI));
		polygonPart->setRotation(_angle);
		polygonPart->setCameraMatrixState(false);
	}
}