
#include "Geometry.h"
#include "Vertex.h"

#include <math.h>
#include <algorithm>

#include <glm/gtx/vector_query.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace spehs
{
	inline float magnitude(const glm::vec2& vec)
	{
		return sqrt(vec.x*vec.x + vec.y*vec.y);
	}
	inline float distance(const glm::vec2& origin, const glm::vec2& destination)
	{
		return magnitude(origin - destination);
	}
	inline float getAngle(glm::vec2& origin, glm::vec2& destination)
	{
		glm::vec2 angle = destination - origin;
		float pointDirection = atan2(angle.y, angle.x);
		if (angle.y < 0)
			pointDirection += TWO_PI;
		return pointDirection;
	}
	inline float getAcuteAngle(const glm::vec2& origin, const glm::vec2& destination)
	{
		//TODO?
		return NULL;
	}
	inline float getAngle(const glm::vec2& destination)
	{
		float pointDirection = atan2(destination.y, destination.x);
		if (destination.y < 0)
			pointDirection += TWO_PI;
		return pointDirection;
	}
	inline float getAngle(const float angle1, const float angle2)
	{
		float angle = angle2 - angle1;
		if (angle < 0)
			angle += TWO_PI;
		return angle;
	}
	inline bool anglesParallel(float angle1, float angle2, const float radTolerance)
	{
		angle2 -= angle1;
		if (angle2 < 0)
			angle2 += TWO_PI;
		if (angle2 < radTolerance || angle2 > TWO_PI - radTolerance)
			return true;
		return false;
	}
	inline glm::vec3 getRotation(const glm::vec3& _a, const glm::vec3& _b)
	{
		glm::vec3 v = glm::cross(_a, _b);
		float c = glm::dot(_a, _b)/(glm::length(_a)*glm::length(_b));

		return glm::eulerAngles(glm::angleAxis(c, v));
	}
	inline float getArea(glm::vec2* cusps, unsigned numCusps)
	{
		if (numCusps < 3)
			return 0.000000001f;
		float area(0.0f);
		int j = numCusps - 1;
		for (int i = 0; i < numCusps; i++)
		{
			area += (cusps[j].x * cusps[i].y) - (cusps[i].x * cusps[j].y);
			j = i;
		}
		return std::max(abs((area * 0.5f)), 0.0001f);
	}
	inline float getRadius(glm::vec2* cusps, unsigned numCusps)
	{
		if (numCusps == 0)
			return 0.0f;
		float max = magnitude(cusps[0]);
		for (unsigned i = 1; i < numCusps; i++)
		{
			if (magnitude(cusps[i]) > max)
				max = magnitude(cusps[i]);
		}
		return max;
	}
	inline glm::vec2 getCenter(spehs::Vertex* cusps, unsigned numCusps)
	{
		glm::vec2 result(0.0f);
		for (unsigned i = 0; i < numCusps; i++)
		{
			result += glm::vec2(cusps[i].position.x, cusps[i].position.y);
		}
		return result / (float)numCusps;
	}
	inline glm::vec2 getCenter(glm::vec2* cusps, unsigned numCusps)
	{
		glm::vec2 result(0.0f);
		for (unsigned i = 0; i < numCusps; i++)
		{
			result += glm::vec2(cusps[i].x, cusps[i].y);
		}
		return result / (float)numCusps;
	}
	inline float cross2(const glm::vec2& _vec1, const glm::vec2& _vec2)
	{
		return _vec1.x * _vec2.y - _vec1.y * _vec2.x;
	}
	inline glm::vec2 cross2(const float& _scalar, const glm::vec2& _vec)
	{
		return glm::vec2(_scalar * _vec.y, -_scalar * _vec.x);
	}
	inline glm::vec2 cross2(const glm::vec2& _vec, const float& _scalar)
	{
		return glm::vec2(-_scalar * _vec.y, _scalar * _vec.x);
	}
	float pointToLineDistance(const glm::vec2 &_begin, const glm::vec2 &_end, const glm::vec2 &_point)
	{
		float normalLength = sqrtf((_end.x - _begin.x) * (_end.x - _begin.x) + (_end.y - _begin.y) * (_end.y - _begin.y));
		return abs((_point.x - _begin.x) * (_end.y - _begin.y) - (_point.y - _begin.y) * (_end.x - _begin.x)) / normalLength;
	}
	glm::vec2* generateCusps(unsigned& numCusps, int shape, float width, float height)
	{
		if (!shape)
		{
			numCusps = 0;
			return nullptr;
		}
		if (shape > 2)
		{//Isosceles shape

			//Generate number of cusps desired
			glm::vec2* cusps = new glm::vec2[shape];

			//firstPosition adjusts initial the rotation for even numbered polygons
			float firstPosition;

			//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
			if (shape % 2)
				firstPosition = HALF_PI;
			else
				firstPosition = HALF_PI + (TWO_PI / shape) / 2;
			cusps[0] = glm::vec2(cos(firstPosition), sin(firstPosition));

			//Take record of min/max cusp values so that it can be streched to match the desired width/height
			float	minX = cusps[0].x,
				minY = cusps[0].y,
				maxX = cusps[0].x,
				maxY = cusps[0].y;
			for (int i = 1; i < shape; i++)
			{
				//Set position
				cusps[i] = glm::vec2(
					cos(firstPosition - i * (TWO_PI / shape)),
					sin(firstPosition - i * (TWO_PI / shape)));

				//Check min/max
				if (cusps[i].x > maxX)
					maxX = cusps[i].x;
				else if (cusps[i].x < minX)
					minX = cusps[i].x;
				if (cusps[i].y > maxY)
					maxY = cusps[i].y;
				else if (cusps[i].y < minY)
					minY = cusps[i].y;
			}

			//Determine width and height factors to scale the cusp locations
			float wFactor = width * 1.0f / (maxX - minX);//Note: Max is positive, min is negative
			float hFactor = height * 1.0f / (maxY - minY);//Note: Max is positive, min is negative

			//Rescale cusps
			for (int i = 0; i < shape; i++)
			{
				cusps[i].x *= wFactor;
				cusps[i].y *= hFactor;
			}

			//Assign cusps quantity to given unsigned reference
			numCusps = shape;
			return cusps;
		}
		else switch (shape)
		{//Predetermine shapes
		default:
			numCusps = 0;
			return nullptr;
		}
	}
}