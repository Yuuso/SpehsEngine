#include "SpehsEngine/Rendering/Arrow.h"
#include "SpehsEngine/Rendering/Line.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Core/Geometry.h"
#include "SpehsEngine/Core/Exceptions.h"


namespace spehs
{
	static std::vector<spehs::vec2> pointerVertices = { spehs::vec2(-0.5f, 0.5f), spehs::vec2(-0.5f, -0.5f), spehs::vec2(0.5f, 0.0f) };
	Arrow::Arrow(BatchManager& batchManager, const int16_t planeDepth) : linePart(nullptr), polygonPart(nullptr), pointerWidth(1.0f), pointerHeight(1.0f), length(0.0f)
	{
		//Create needed primitive components
		polygonPart = batchManager.createPolygon(pointerVertices, planeDepth, pointerWidth, pointerHeight);
		polygonPart->setCameraMatrixState(false);
		linePart = batchManager.createLine(spehs::vec2::zero, spehs::vec2::zero, planeDepth);
		linePart->setCameraMatrixState(false);
		linePart->setRenderState(false);//Length == 0
	}
	Arrow::Arrow(BatchManager& batchManager, const spehs::vec2& _startPoint, const spehs::vec2& _endPoint, const int16_t planeDepth) : Arrow(batchManager)
	{
		setPosition(_startPoint, _endPoint);
	}
	Arrow::~Arrow()
	{
		linePart->destroy();
		polygonPart->destroy();
	}


	void Arrow::setPosition(const spehs::vec2& _startPoint, const spehs::vec2& _endPoint)
	{
		float angle = atan2(_endPoint.y - _startPoint.y, _endPoint.x - _startPoint.x);
		spehs::vec2 arrowVector(_endPoint - _startPoint);

		//Arrow length changed
		length = magnitude(arrowVector);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(polygonPart->getRenderState());

		//End of line position
		arrowVector -= spehs::vec2(cos(angle) * pointerHeight, sin(angle) * pointerHeight);

		//Primitive positioning
		linePart->setPoints(_startPoint, _startPoint + arrowVector);
		polygonPart->setPosition((_startPoint.x + arrowVector.x + _endPoint.x) * 0.5f, (_startPoint.y + arrowVector.y + _endPoint.y) * 0.5f);

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
	void Arrow::setArrowPointerSize(const spehs::vec2 &_ref)
	{
		pointerWidth = _ref.x;
		pointerHeight = _ref.y;
		polygonPart->resize(pointerHeight, pointerWidth);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(polygonPart->getRenderState());
	}

	void Arrow::setColor(const spehs::Color& _col)
	{
		linePart->setColor(_col);
		polygonPart->setColor(_col);
	}

	void Arrow::setAlpha(const float _alpha)
	{
		linePart->setAlpha(_alpha);
		polygonPart->setAlpha(_alpha);
	}

	void Arrow::setRenderState(const bool _value)
	{
		polygonPart->setRenderState(_value);
		if (length < pointerHeight)
			linePart->setRenderState(false);
		else
			linePart->setRenderState(_value);
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