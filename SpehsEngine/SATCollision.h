#pragma once

#include "Vertex.h"

#include <glm\vec2.hpp>
#include <vector>
#include <memory>


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
	struct CollisionPoint
	{
		CollisionPoint();
		~CollisionPoint();
		glm::vec2 MTV;
		std::vector<glm::vec2> point;
		std::vector<glm::vec2> normal;
	};

	//Function for projecting polygon onto axis
	Projection projectPolygon(const glm::vec2& _axis, Vertex* _vertexArray, const unsigned int _size);
	Projection projectPolygon(const glm::vec2& _axis, Position* _vertexArray, const unsigned int _size);

	//Function for projecting circle onto axis
	Projection projectCircle(const glm::vec2& _axis, const glm::vec2& _vertex, const float _radius);

	//Function for getting axis
	glm::vec2 getAxis(Vertex* _vertexArray, const unsigned int _size, const unsigned int _i);
	glm::vec2 getAxis(Position* _vertexArray, const unsigned int _size, const unsigned int _i);

	//Get the axis for circle
	glm::vec2 getCircleAxis(Vertex* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenter);
	glm::vec2 getCircleAxis(Position* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenter);

	//BOOL COLLISIONS
	//Polygon x Polygon collision
	bool SATCollision(Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2);
	bool SATCollision(Position* _vertexArray1, const unsigned int _size1, Position* _vertexArray2, const unsigned int _size2);

	//Polygon x Circle Collision
	bool SATCollision(Vertex* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius);
	bool SATCollision(Position* _vertexArray, unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleCollision(const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2);

	//Ray x Circle collision
	bool rayCollision(const glm::vec2 _rayPosition, const float _rayDirection, const glm::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = RAYCAST_DISTANCE);
	float rayCollisionDistance(const glm::vec2 _rayPosition, const float _rayDirection, const glm::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = RAYCAST_DISTANCE);

	//Ray x Polygon collision, returns 0.0f if no collision, otherwise > 0 (collision distance from point of ray origin)
	bool rayCollision(const glm::vec2 _rayCastPosition, float _rayDirection, spehs::Position* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = RAYCAST_DISTANCE);
	float rayCollisionDistance(const glm::vec2 _rayCastPosition, float _rayDirection, spehs::Position* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = RAYCAST_DISTANCE);

	//COLLISIONPOINT COLLISIONS
	//Polygon x Polygon collision
	std::shared_ptr<CollisionPoint> SATMTVCollision(Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2);
	std::shared_ptr<CollisionPoint> SATMTVCollision(Position* _vertexArray1, const unsigned int _size1, Position* _vertexArray2, const unsigned int _size2);

	//Polygon x Circle Collision
	std::shared_ptr<CollisionPoint> SATMTVCollision(Vertex* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius);
	std::shared_ptr<CollisionPoint> SATMTVCollision(Position* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	std::shared_ptr<CollisionPoint> circleMTVCollision(const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2);
}