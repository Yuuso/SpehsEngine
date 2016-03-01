#pragma once

#include "Primitive.h"

#include <glm/vec2.hpp>


#define DISTANCE_EPSILON 0.0000001f
typedef unsigned int GLuint;

namespace SpehsEngine
{
	class Line : public Primitive
	{
	public:
		Line(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth = 0);
		Line(const glm::vec3 &_startPoint, const glm::vec3 &_endPoint);
		//Line(const float &_lenght, const float &_angle); //Starting point is at 0,0

		Line* getLinePtr(){ return this; }

		void updateVertices();
		void setPoints(const glm::vec2 &_newStartPoint, const glm::vec2 &_newEndPoint);
		void setPoints(const glm::vec3 &_newStartPoint, const glm::vec3 &_newEndPoint);
		//void setPoints(const float &_newLenght, const float &_newAngle);
		void setThickness(const float &_newThickness);

	protected:
		~Line();
		float thickness;
	};
}
