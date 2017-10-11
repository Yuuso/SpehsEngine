#pragma once

#include "Vertex.h"
#include "Vector.h"

#include <vector>


const float RAYCAST_DISTANCE = FLT_MAX / 2.0f;


namespace spehs
{
	struct Projection
	{
		Projection(const float _min, const float _max) : min(_min), max(_max){}
		~Projection() {}

		const float min = 0.0f, max = 0.0f;
		bool overlap(const Projection& _p)
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
		float getOverlap(const Projection& _p)
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
	struct CollisionResults
	{
		spehs::vec2 MTV;
		struct Point
		{
			Point() = default;
			Point(const spehs::vec2& _position, const spehs::vec2& _normal) : position(_position), normal(_normal) {}
			spehs::vec2 position;
			spehs::vec2 normal;
		};
		std::vector<Point> points;
	};

	//Function for projecting polygon onto axis
	Projection projectPolygon(const spehs::vec2& _axis, Vertex* _vertexArray, const unsigned int _size);
	Projection projectPolygon(const spehs::vec2& _axis, spehs::vec2* _vertexArray, const unsigned int _size);

	//Function for projecting circle onto axis
	Projection projectCircle(const spehs::vec2& _axis, const spehs::vec2& _vertex, const float _radius);

	//Function for getting axis
	spehs::vec2 getAxis(Vertex* _vertexArray, const unsigned int _size, const unsigned int _i);
	spehs::vec2 getAxis(spehs::vec2* _vertexArray, const unsigned int _size, const unsigned int _i);

	//Get the axis for circle
	spehs::vec2 getCircleAxis(Vertex* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenter);
	spehs::vec2 getCircleAxis(spehs::vec2* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenter);

	//BOOL COLLISIONS
	//Polygon x Polygon collision
	bool SATCollision(Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2);
	bool SATCollision(spehs::vec2* _vertexArray1, const unsigned int _size1, spehs::vec2* _vertexArray2, const unsigned int _size2);

	//Polygon x Circle Collision
	bool SATCollision(Vertex* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius);
	bool SATCollision(spehs::vec2* _vertexArray, unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleCollision(const spehs::vec2& _circleCenterPoint1, const float _circleRadius1, const spehs::vec2& _circleCenterPoint2, const float _circleRadius2);

	//Ray x Circle collision
	bool rayCollision(const spehs::vec2 _rayPosition, const float _rayDirection, const spehs::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = RAYCAST_DISTANCE);
	float rayCollisionDistance(const spehs::vec2 _rayPosition, const float _rayDirection, const spehs::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = RAYCAST_DISTANCE);

	//Ray x Polygon collision, returns 0.0f if no collision, otherwise > 0 (collision distance from point of ray origin)
	bool rayCollision(const spehs::vec2 _rayCastPosition, float _rayDirection, spehs::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = RAYCAST_DISTANCE);
	float rayCollisionDistance(const spehs::vec2 _rayCastPosition, float _rayDirection, spehs::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = RAYCAST_DISTANCE);

	//COLLISIONPOINT COLLISIONS
	//Polygon x Polygon collision
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2);
	bool SATMTVCollision(CollisionResults& deposit, spehs::vec2* _vertexArray1, const unsigned int _size1, spehs::vec2* _vertexArray2, const unsigned int _size2);

	//Polygon x Circle Collision
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius);
	bool SATMTVCollision(CollisionResults& deposit, spehs::vec2* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleMTVCollision(CollisionResults& deposit, const spehs::vec2& _circleCenterPoint1, const float _circleRadius1, const spehs::vec2& _circleCenterPoint2, const float _circleRadius2);
}