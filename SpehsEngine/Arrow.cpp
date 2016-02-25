#define PI 3.14159265358979323846264338

#include "Arrow.h"
#include "Exceptions.h"


namespace SpehsEngine
{
	Arrow::Arrow()
	{//Default constructor

	}
	Arrow::Arrow(glm::vec2& _startPoint, glm::vec2& _endPoint)
	{
		createArrow(_startPoint, _endPoint);
	}
	Arrow::~Arrow()
	{
		if (linePart)
			delete linePart;
		if (polygonPart)
			delete polygonPart;
	}


	void Arrow::setPosition(glm::vec2& _startPoint, glm::vec2& _endPoint)
	{
		createArrow(_startPoint, _endPoint);
	}


	void Arrow::setLineThickness(float _thickness)
	{
		if (linePart == nullptr)
		{
			return;
		}
		linePart->setThickness(_thickness);
	}


	void Arrow::setArrowPointerSize(float _sizeX, float _sizeY)
	{
		arrowSizeX = _sizeX;
		arrowSizeY = _sizeY;
	}
	void Arrow::setArrowPointerSize(glm::vec2& _ref)
	{
		arrowSizeX = _ref.x;
		arrowSizeY = _ref.y;
	}


	void Arrow::setArrowColor(int _r, int _b, int _g, int _a)
	{
		if (linePart)
			linePart->setColor(_r, _b, _g, _a);
		if (polygonPart)
			polygonPart->setColor(_r, _b, _g, _a);
	}


	void Arrow::setArrowColor(glm::vec4& _color)
	{
		if (linePart)
			linePart->setColor(_color);
		if (polygonPart)
			polygonPart->setColor(_color);
	}


	void Arrow::render()
	{
		if (linePart)
			linePart->draw();
		if (polygonPart)
			polygonPart->draw();
	}


	//Private:
	void Arrow::createArrow(glm::vec2& _startPoint, glm::vec2& _endPoint)
	{
		if (_startPoint == _endPoint)
			return;

		if (linePart != nullptr)
			delete linePart;
		if (polygonPart != nullptr)
			delete polygonPart;

		linePart = new LineBatch(_startPoint, _endPoint);
		linePart->setCameraMatrixState(false);

		float _angle = -atan2(_endPoint.x - _startPoint.x, _endPoint.y - _startPoint.y);

		polygonPart = new PolygonBatch(3, arrowSizeX, arrowSizeY);
		polygonPart->setPosition(_endPoint.x + 0.5f*arrowSizeY*cos(_angle - 0.5f * PI), _endPoint.y + 0.5f*arrowSizeY*sin(_angle - 0.5f * PI));
		polygonPart->setRotation(_angle);
		polygonPart->setCameraMatrixState(false);
	}
}