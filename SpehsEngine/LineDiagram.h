#pragma once
#include <vector>
#include "Vector.h"
#include "Color.h"

namespace spehs
{
	class BatchManager;
	class Polygon;
	class Line;

	/*
		(The line diagram works a bit like a float vector.)
	*/
	class LineDiagram
	{
	public:
		LineDiagram(BatchManager& batchManager);
		~LineDiagram();

		//Point data
		void set(const int index, const float value);
		const float get(const int index) const;
		void pushBack(const float value);
		void insert(const int index, const float value);
		void erase(const int index);
		size_t getPointCount() const;
		void setCapacity(const size_t newCapacity);
		size_t getCapacity() const;

		//Visual state
		void setSize(const float width, const float height);
		void setPosition(const float x, const float y);
		vec2 getSize() const;
		float getWidth() const;
		float getHeight() const;
		vec2 getPosition() const;
		float getX() const;
		float getY() const;

	private:
		void updateLineVisuals();
		void trimToCapacity();

		BatchManager& batchManager;
		Polygon* polygon;//NOTE: polygon holds size and position data!
		struct PointData
		{
			float value;
			Line* line;
		};
		std::vector<PointData> points;
		size_t capacity;
		Color lineColor;
	};
}