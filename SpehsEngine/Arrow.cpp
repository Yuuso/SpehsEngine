#define PI 3.14159265358979323846264338

#include "Arrow.h"
#include "Line.h"
#include "Polygon.h"
#include "Exceptions.h"
#include "BatchManager.h"


namespace spehs
{
	Arrow::Arrow()
	{
		//Create needed primitive components
		polygonPart = getActiveBatchManager()->createPolygon(3, 0, arrowSizeX, arrowSizeY);
		polygonPart->setCameraMatrixState(false);
		linePart = getActiveBatchManager()->createLine(glm::vec2(0), glm::vec2(0), 0);
		linePart->setCameraMatrixState(false);
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


	void Arrow::setPosition(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint)
	{
		//Primitive positioning
		linePart->setPoints(_startPoint, _endPoint);
		polygonPart->setPosition(_endPoint.x, _endPoint.y);

		//Polygon rotation
		float _angle = -atan2(_endPoint.x - _startPoint.x, _endPoint.y - _startPoint.y);
		polygonPart->setRotation(_angle);
	}


	void Arrow::setLineThickness(const float _thickness)
	{
		//TODO: set line thickness
	}


	void Arrow::setArrowPointerSize(const float _sizeX, const float _sizeY)
	{
		polygonPart->resize(_sizeX, _sizeY);
	}
	void Arrow::setArrowPointerSize(const glm::vec2 &_ref)
	{
		polygonPart->resize(_ref.x, _ref.y);
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
		linePart->setRenderState(_value);
		polygonPart->setRenderState(_value);
	}


	void Arrow::setDepth(const PlaneDepth _value)
	{
		linePart->setPlaneDepth(_value);
		polygonPart->setPlaneDepth(_value);
	}


	void Arrow::cameraState(const bool _value)
	{
		linePart->setCameraMatrixState(_value);
		polygonPart->setCameraMatrixState(_value);
	}

}