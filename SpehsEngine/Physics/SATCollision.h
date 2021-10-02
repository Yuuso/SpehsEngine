#pragma once

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
		glm::vec2 MTV;
		struct Point
		{
			Point() = default;
			Point(const glm::vec2& _position, const glm::vec2& _normal) : position(_position), normal(_normal) {}
			glm::vec2 position;
			glm::vec2 normal;
		};
		std::vector<Point> points;
	};

	//Function for projecting polygon onto axis
	Projection projectPolygon(const glm::vec2& _axis, glm::vec2* _vertexArray, const size_t _size);

	//Function for projecting circle onto axis
	Projection projectCircle(const glm::vec2& _axis, const glm::vec2& _vertex, const float _radius);

	//Function for getting axis
	glm::vec2 getAxis(glm::vec2* _vertexArray, const size_t _size, const size_t _i);

	//Get the axis for circle
	glm::vec2 getCircleAxis(glm::vec2* _vertexArray, const size_t _size, const glm::vec2& _circleCenter);

	//BOOL COLLISIONS
	//Polygon x Polygon collision
	bool SATCollision(const glm::vec2* _vertexArray1, const size_t _size1, const glm::vec2* _vertexArray2, const size_t _size2);

	//Polygon x Circle Collision
	bool SATCollision(glm::vec2* _vertexArray, const size_t _size, const glm::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleCollision(const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2);

	//Ray x Circle collision
	bool rayCollision(const glm::vec2 _rayPosition, const float _rayDirection, const glm::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);
	float rayCollisionDistance(const glm::vec2 _rayPosition, const float _rayDirection, const glm::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);

	//Ray x Polygon collision, returns 0.0f if no collision, otherwise > 0 (collision distance from point of ray origin)
	bool rayCollision(const glm::vec2 _rayCastPosition, float _rayDirection, glm::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);
	float rayCollisionDistance(const glm::vec2 _rayCastPosition, float _rayDirection, glm::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance = DEFAULT_RAYCAST_DISTANCE);

	//COLLISIONPOINT COLLISIONS
	//Polygon x Polygon collision
	bool SATMTVCollision(CollisionResults& deposit, glm::vec2* _vertexArray1, const size_t _size1, glm::vec2* _vertexArray2, const size_t _size2);

	//Polygon x Circle Collision
	bool SATMTVCollision(CollisionResults& deposit, glm::vec2* _vertexArray, const size_t _size, const glm::vec2& _circleCenterPoint, const float _circleRadius);

	//Circle x Circle Collision
	bool circleMTVCollision(CollisionResults& deposit, const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2);
}