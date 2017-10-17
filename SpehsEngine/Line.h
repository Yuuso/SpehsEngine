#pragma once

#include "Primitive.h"


typedef unsigned int GLuint;

namespace spehs
{
	class Line : public Primitive
	{
		friend class BatchManager;

	public:
		static Line* create(const spehs::vec2& _startPoint, const spehs::vec2& _endPoint, const PlaneDepth& _planeDepth);
		static Line* create(const PlaneDepth& _planeDepth);

		Line* getLinePtr() override { return this; }

		void updateVertices();
		void setPoints(const spehs::vec2& _newStartPoint, const spehs::vec2& _newEndPoint);
		//void setPoints(const float &_newLength, const float &_newAngle);

	protected:
		Line(const PlaneDepth& _planeDepth);
		Line(const spehs::vec2& _startPoint, const spehs::vec2& _endPoint, const PlaneDepth& _planeDepth);
		//Line(const float &_length, const float &_angle); //Starting point is at 0,0
		~Line();
	};
}
