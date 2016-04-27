#pragma once

#include "Vertex.h"

#include <glm\vec2.hpp>
#include <vector>


namespace spehs
{
	struct Projection
	{
		Projection(float _min, float _max) : min(_min), max(_max){}
		~Projection() {}

		float min = 0.0f, max = 0.0f;
		bool overlap(Projection& _p)
		{
			if (this->min >= _p.min && this->min <= _p.max)
			{
				return true;
			}
			else if (this->max >= _p.min && this->max <= _p.max)
			{
				return true;
			}
			else if (this->min < _p.min && this->max > _p.max)
			{
				return true;
			}
			return false;
		}
		float getOverlap(Projection& _p)
		{
			if (this->min >= _p.min && this->min <= _p.max)
			{
				return _p.max - this->min;
			}
			else if (this->max >= _p.min && this->max <= _p.max)
			{
				return this->max - _p.min;
			}
			else if (this->min < _p.min && this->max > _p.max)
			{
				if ((this->max - _p.max) < (_p.min - this->min))
				{
					return this->max - _p.max;
				}
				return _p.min - this->min;
			}
			return NULL;
		}
	};
	struct CollisionPoint
	{
		glm::vec2 MTV;
		std::vector<glm::vec2> point;
		std::vector<glm::vec2> normal;
	};

	//Function for projecting polygon onto axis
	Projection projectPolygon(glm::vec2 _axis, Vertex* _vertexArray, int _size);
	Projection projectPolygon(glm::vec2 _axis, Position* _vertexArray, int _size);

	//Function for projecting circle onto axis
	Projection projectCircle(glm::vec2 _axis, glm::vec2& _vertex, float _radius);

	//Function for getting axis
	glm::vec2 getAxis(Vertex* _vertexArray, int _size, int _i);
	glm::vec2 getAxis(Position* _vertexArray, int _size, int _i);

	//Get the axis for circle
	glm::vec2 getCircleAxis(Vertex* _vertexArray, int _size, glm::vec2& _circleCenter);
	glm::vec2 getCircleAxis(Position* _vertexArray, int _size, glm::vec2& _circleCenter);

	//Line Collision
	bool isIntersecting(const glm::vec2& _start1, const glm::vec2& _end1, const glm::vec2& _start2, const glm::vec2& _end2);


	//Polygon x Polygon collision
	bool SATCollision(Vertex* _vertexArray1, int _size1, Vertex* _vertexArray2, int _size2);
	CollisionPoint* SATMTVCollision(Vertex* _vertexArray1, int _size1, Vertex* _vertexArray2, int _size2);
	bool SATCollision(Position* _vertexArray1, int _size1, Position* _vertexArray2, int _size2);
	CollisionPoint* SATMTVCollision(Position* _vertexArray1, int _size1, Position* _vertexArray2, int _size2);

	//Polygon x Circle Collision
	bool SATCollision(Vertex* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius);
	CollisionPoint* SATMTVCollision(Vertex* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius);
	bool SATCollision(Position* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius);
	CollisionPoint* SATMTVCollision(Position* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius);

	//Circle x Circle Collision
	bool CircleCollision(glm::vec2& _circleCenterPoint1, float _circleRadius1, glm::vec2& _circleCenterPoint2, float _circleRadius2);
	CollisionPoint* CircleMTVCollision(glm::vec2& _circleCenterPoint1, float _circleRadius1, glm::vec2& _circleCenterPoint2, float _circleRadius2);
}