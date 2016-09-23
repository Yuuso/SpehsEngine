#pragma once

#include "Primitive.h"

#include <glm/vec2.hpp>


#define DISTANCE_EPSILON 0.00001f
typedef unsigned int GLuint;

namespace spehs
{
	class Line : public Primitive
	{
		friend class BatchManager;

	public:
		Line* getLinePtr(){ return this; }

		void updateVertices();
		void setPoints(const glm::vec2 &_newStartPoint, const glm::vec2 &_newEndPoint);
		//void setPoints(const float &_newLenght, const float &_newAngle);

	protected:
		Line(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth);
		//Line(const float &_lenght, const float &_angle); //Starting point is at 0,0
		~Line();
	};
}
