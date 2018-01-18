#pragma once
#include <vector>
#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Core/Color.h"

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
		float get(const int index) const;
		void pushBack(const float value);
		void insert(const int index, const float value);
		void erase(const int index);
		size_t getPointCount() const;
		void setCapacity(const size_t newCapacity);
		size_t getCapacity() const;

		void setRenderState(const bool state);
		void setSize(const float width, const float height);
		void setPosition(const float x, const float y);
		void setDepth(const int16_t depth);
		void setBackgroundColor(const Color& color);
		void setLineColor(const Color& color);
		bool getRenderState() const;
		int16_t getDepth() const;
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


	class LineDiagram2
	{
	public:
		LineDiagram2(BatchManager& batchManager);
		~LineDiagram2();

		//Point data
		void set(const int index, const spehs::vec2& point);
		spehs::vec2 get(const int index) const;
		void pushBack(const spehs::vec2& point);
		void insert(const int index, const spehs::vec2& point);
		void erase(const int index);
		size_t getPointCount() const;
		void setCapacity(const size_t newCapacity);
		size_t getCapacity() const;

		void setRenderState(const bool state);
		void setSize(const float width, const float height);
		void setPosition(const float x, const float y);
		void setDepth(const int16_t depth);
		void setBackgroundColor(const Color& color);
		void setLineColor(const Color& color);
		bool getRenderState() const;
		int16_t getDepth() const;
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
			spehs::vec2 point;
			Line* line = nullptr;
		};
		std::vector<PointData> points;
		size_t capacity;
		Color lineColor;
	};
}