#include "SpehsEngine/Rendering/LineDiagram.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/Line.h"

namespace spehs
{
	LineDiagram::LineDiagram(BatchManager& _batchManager)
		: batchManager(_batchManager)
		, polygon(nullptr)
		, capacity(100)
		, lineColor(128, 255, 0)
	{
		polygon = batchManager.createPolygon(Shape::BUTTON, PlaneDepth(0), 100.0f, 100.0f);
		polygon->setCameraMatrixState(false);
		polygon->setColor(Color(128, 128, 128));
	}

	LineDiagram::~LineDiagram()
	{
		if (polygon)
			polygon->destroy();
		for (size_t i = 0; i < points.size(); i++)
		{
			if (points[i].line)
				points[i].line->destroy();
		}
	}

	void LineDiagram::set(const int index, const float value)
	{
		points[index].value = value;
		updateLineVisuals();
	}

	const float LineDiagram::get(const int index) const
	{
		return points[index].value;
	}

	void LineDiagram::pushBack(const float value)
	{
		points.push_back(PointData());
		points.back().value = value;
		trimToCapacity();
		updateLineVisuals();
	}

	void LineDiagram::insert(const int index, const float value)
	{
		points.insert(points.begin() + index, PointData());
		points[index].value = value;
		trimToCapacity();
	}

	void LineDiagram::erase(const int index)
	{
		points.erase(points.begin() + index);
		updateLineVisuals();
	}

	size_t LineDiagram::getPointCount() const
	{
		return points.size();
	}

	void LineDiagram::setCapacity(const size_t newCapacity)
	{
		capacity = newCapacity;
		trimToCapacity();
	}

	size_t LineDiagram::getCapacity() const
	{
		return capacity;
	}

	void LineDiagram::trimToCapacity()
	{
		if (points.size() > capacity)
		{
			const size_t removeCount = points.size() - capacity;
			points.erase(points.begin(), points.begin() + removeCount);
			updateLineVisuals();
		}
	}

	void LineDiagram::setRenderState(const bool state)
	{
		if (state != polygon->getRenderState())
		{
			polygon->setRenderState(state);
			for (size_t i = 0; i < points.size(); i++)
			{
				if (points[i].line)
					points[i].line->setRenderState(state);
			}
		}
	}

	void LineDiagram::setDepth(const int16_t depth)
	{
		if (depth != polygon->getPlaneDepth())
		{
			polygon->setPlaneDepth(depth);
			for (size_t i = 0; i < points.size(); i++)
			{
				if (points[i].line)
					points[i].line->setPlaneDepth(depth + 1);
			}
		}
	}

	void LineDiagram::setBackgroundColor(const Color& color)
	{
		polygon->setColor(color);
	}

	void LineDiagram::setLineColor(const Color& color)
	{
		lineColor = color;
		for (size_t i = 0; i < points.size(); i++)
		{
			if (points[i].line)
				points[i].line->setColor(color);
		}
	}

	void LineDiagram::setSize(const float width, const float height)
	{
		const Color col = polygon->getColor();
		const vec2 pos = polygon->getPosition();
		polygon->destroy();
		polygon = batchManager.createPolygon(Shape::BUTTON, (PlaneDepth)0, width, height);
		polygon->setCameraMatrixState(false);
		polygon->setColor(col);
		polygon->setPosition(pos);
		updateLineVisuals();
	}

	void LineDiagram::setPosition(const float x, const float y)
	{
		polygon->setPosition(x, y);
		updateLineVisuals();
	}

	bool LineDiagram::getRenderState() const
	{
		return polygon->getRenderState();
	}

	int16_t LineDiagram::getDepth() const
	{
		return polygon->getPlaneDepth();
	}

	vec2 LineDiagram::getSize() const
	{
		return vec2(polygon->getWidth(), polygon->getHeight());
	}

	float LineDiagram::getWidth() const
	{
		return polygon->getWidth();
	}

	float LineDiagram::getHeight() const
	{
		return polygon->getHeight();
	}

	vec2 LineDiagram::getPosition() const
	{
		return polygon->getPosition();
	}

	float LineDiagram::getX() const
	{
		return polygon->getX();
	}

	float LineDiagram::getY() const
	{
		return polygon->getY();
	}

	void LineDiagram::updateLineVisuals()
	{

		//Get min/max visible values
		float minValue = 0.0f;
		float maxValue = 0.0f;
		for (size_t p = 0; p < points.size(); p++)
		{
			if (points[p].value < minValue)
				minValue = points[p].value;
			else if (points[p].value > maxValue)
				maxValue = points[p].value;
		}

		PointData* previous = nullptr;
		const float interval = polygon->getWidth() / (points.size() + 1);
		const float variance = maxValue - minValue;
		for (size_t p = 0; p < points.size(); p++)
		{
			if (previous)
			{//Previous point exists

				//Connecting line to previous
				if (!points[p].line)
				{
					points[p].line = batchManager.createLine(polygon->getPlaneDepth());
					points[p].line->setRenderState(polygon->getRenderState());
					points[p].line->setCameraMatrixState(false);
					points[p].line->setLineWidth(1.0f);
					points[p].line->setPlaneDepth(polygon->getPlaneDepth() + 1);
					points[p].line->setColor(lineColor);
				}

				const float prevHeight = (previous->value - minValue) / variance;
				const float thisHeight = (points[p].value - minValue) / variance;
				const vec2 begin = polygon->getPosition() + vec2(p * interval, polygon->getHeight() * prevHeight);
				const vec2 end = polygon->getPosition() + vec2((p + 1) * interval, polygon->getHeight() * thisHeight);
				points[p].line->setPoints(begin, end);
			}
			else
			{//No previous point exists
				if (points[p].line)
					points[p].line->destroy();
				points[p].line = nullptr;
			}
			previous = &points[p];
		}
	}
}