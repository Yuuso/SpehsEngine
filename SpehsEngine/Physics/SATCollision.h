#pragma once

#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Core/Vector.h"

#include <vector>
#include <limits>



namespace se
{
	constexpr float DEFAULT_RAYCAST_DISTANCE = std::numeric_limits<float>::max() / 2.0f;

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
		se::vec2 MTV;
		struct Point
		{
			Point() = default;
			Point(const se::vec2& _position, const se::vec2& _normal) : position(_position), normal(_normal) {}
			se::vec2 position;
			se::vec2 normal;
		};
		std::vector<Point> points;
	};

	//Function for projecting polygon onto axis
	Projection projectPolygon(const se::vec2& _axis, Vertex* _vertexArray, const size_t _size);
	Projection projectPolygon(const se::vec2& _axis, se::vec2* _vertexArray, const size_t _size);

	//Function for projecting circle onto axis
	Projection projectCircle(const se::vec2& _axis, const se::vec2& _vertex, const float _radius);

	//Function for getting axis
	se::vec2 getAxis(Vertex* _vertexArray, const size_t _size, const size_t _i);
	se::vec2 getAxis(se::vec2* _vertexArray, const size_t _size, const size_t _i);

	//Get the axis for circle
	se::vec2 getCircleAxis(Vertex* _vertexArray, const size_t _size, const se::vec2& _circleCenter);
	se::vec2 getCircleAxis(se::vec2* _vertexArray, const size_t _size, const se::vec2& _circleCenter);

	//BOOL COLLISIONS
	//Polygon x Polygon collision
	bool SATCollision(Vertex* _vertexArray1, const size_t _size1, Vertex* _vertexArray2, const size_t _size2);
	bool SATCollision(const se::vec2* _vertexArray1, const size_t _size1, const se::vec2* _vertexArray2, const size_t _size2);

	//Polygon x Circle Collision
	bool SATCollision(Vertex* _vertexArray, const size_t _size, const se::vec2& _circleCenterPoint, const float _circleRadius);
	bool SATCollision(se::vec2* _vertexArray, const size_t _size, const se::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleCollision(const se::vec2& _circleCenterPoint1, const float _circleRadius1, const se::vec2& _circleCenterPoint2, const float _circleRadius2);

	//Ray x Circle collision
	bool rayCollision(const se::vec2 _rayPosition, const float _rayDirection, const se::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);
	float rayCollisionDistance(const se::vec2 _rayPosition, const float _rayDirection, const se::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);

	//Ray x Polygon collision, returns 0.0f if no collision, otherwise > 0 (collision distance from point of ray origin)
	bool rayCollision(const se::vec2 _rayCastPosition, float _rayDirection, se::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);
	float rayCollisionDistance(const se::vec2 _rayCastPosition, float _rayDirection, se::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);

	//COLLISIONPOINT COLLISIONS
	//Polygon x Polygon collision
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray1, const size_t _size1, Vertex* _vertexArray2, const size_t _size2);
	bool SATMTVCollision(CollisionResults& deposit, se::vec2* _vertexArray1, const size_t _size1, se::vec2* _vertexArray2, const size_t _size2);

	//Polygon x Circle Collision
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray, const size_t _size, const se::vec2& _circleCenterPoint, const float _circleRadius);
	bool SATMTVCollision(CollisionResults& deposit, se::vec2* _vertexArray, const size_t _size, const se::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleMTVCollision(CollisionResults& deposit, const se::vec2& _circleCenterPoint1, const float _circleRadius1, const se::vec2& _circleCenterPoint2, const float _circleRadius2);
}