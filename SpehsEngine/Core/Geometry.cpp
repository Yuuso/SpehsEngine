#include "stdafx.h"

#include "SpehsEngine/Core/Geometry.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Core/Log.h"

#include <math.h>
#include <algorithm>

#include <glm/gtx/vector_query.hpp>
#include <glm/gtc/type_ptr.hpp>



namespace se
{
	inline float magnitude(const se::vec2& vec)
	{
		return sqrt(vec.x*vec.x + vec.y*vec.y);
	}

	inline float distance(const se::vec2& origin, const se::vec2& destination)
	{
		return magnitude(origin - destination);
	}

	inline float getAngle(const se::vec2& origin, const se::vec2& destination)
	{
		se::vec2 angle = destination - origin;
		float pointDirection = atan2(angle.y, angle.x);
		if (angle.y < 0)
			pointDirection += TWO_PI;
		return pointDirection;
	}

	inline float getAcuteAngle(const se::vec2& origin, const se::vec2& destination)
	{
		//TODO?
		return NULL;
	}

	inline float getAngle(const se::vec2& destination)
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

	inline se::vec3 getRotation(const se::vec3& _a, const se::vec3& _b)
	{
		se_assert(false && "Unimplemented engine function");
		//se::vec3 v = glm::cross(_a, _b);
		//float c = glm::dot(_a, _b)/(glm::length(_a)*glm::length(_b));

		//return glm::eulerAngles(glm::angleAxis(c, v));
		return se::vec3::zero;
	}

	inline void rotateAroundAxis(se::vec2& point, const se::vec2& axis, float rotation)
	{//Translates around the point
		float dx = point.x - axis.x;
		float dy = point.y - axis.y;
		point.x = axis.x + dx*cos(rotation) - dy*sin(rotation);
		point.y = axis.y + dx*sin(rotation) + dy*cos(rotation);
	}

	inline void rotateAroundAxis(se::vec2& point, const float axisX, const float axisY, float rotation)
	{//Translates around the point
		float dx = point.x - axisX;
		float dy = point.y - axisY;
		point.x = axisX + dx*cos(rotation) - dy*sin(rotation);
		point.y = axisY + dx*sin(rotation) + dy*cos(rotation);
	}

	inline void rotateAroundOrigin(se::vec2& point, float rotation)
	{//Translates around origin
		float dx = point.x;
		float dy = point.y;
		point.x = dx*cos(rotation) - dy*sin(rotation);
		point.y = dx*sin(rotation) + dy*cos(rotation);
	}

	inline float getArea(se::vec2* cusps, const size_t numCusps)
	{
		if (numCusps < 3)
			return 0.000000001f;
		float area(0.0f);
		size_t j = numCusps - 1;
		for (size_t i = 0; i < numCusps; i++)
		{
			area += (cusps[j].x * cusps[i].y) - (cusps[i].x * cusps[j].y);
			j = i;
		}
		return std::max(std::abs(area * 0.5f), 0.0001f);
	}

	inline float getRadius(se::vec2* cusps, const size_t numCusps)
	{
		if (numCusps == 0)
			return 0.0f;
		float max = magnitude(cusps[0]);
		for (size_t i = 1; i < numCusps; i++)
		{
			if (magnitude(cusps[i]) > max)
				max = magnitude(cusps[i]);
		}
		return max;
	}

	inline se::vec2 getCenter(se::Vertex* cusps, const size_t numCusps)
	{
		se::vec2 result(0.0f, 0.0f);
		for (size_t i = 0; i < numCusps; i++)
		{
			result += se::vec2(cusps[i].position.x, cusps[i].position.y);
		}
		return result / (float)numCusps;
	}

	inline se::vec2 getCenter(se::vec2* cusps, const size_t numCusps)
	{
		se::vec2 result(0.0f, 0.0f);
		for (size_t i = 0; i < numCusps; i++)
		{
			result += se::vec2(cusps[i].x, cusps[i].y);
		}
		return result / (float)numCusps;
	}

	inline float cross2(const se::vec2& _vec1, const se::vec2& _vec2)
	{
		return _vec1.x * _vec2.y - _vec1.y * _vec2.x;
	}

	inline se::vec2 cross2(const float& _scalar, const se::vec2& _vec)
	{
		return se::vec2(_scalar * _vec.y, -_scalar * _vec.x);
	}

	inline se::vec2 cross2(const se::vec2& _vec, const float& _scalar)
	{
		return se::vec2(-_scalar * _vec.y, _scalar * _vec.x);
	}

	float pointToLineDistance(const se::vec2 &_begin, const se::vec2 &_end, const se::vec2 &_point)
	{
		float normalLength = sqrtf((_end.x - _begin.x) * (_end.x - _begin.x) + (_end.y - _begin.y) * (_end.y - _begin.y));
		return abs((_point.x - _begin.x) * (_end.y - _begin.y) - (_point.y - _begin.y) * (_end.x - _begin.x)) / normalLength;
	}

	se::vec2* generateCusps(size_t& numCusps, int shape, float width, float height)
	{
		if (!shape)
		{
			numCusps = 0;
			return nullptr;
		}
		if (shape > 2)
		{//Isosceles shape

			//Generate number of cusps desired
			se::vec2* cusps = new se::vec2[shape];

			//firstPosition adjusts initial the rotation for even numbered polygons
			float firstPosition;

			//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
			if (shape % 2)
				firstPosition = 0;
			else
				firstPosition = (TWO_PI / shape) / 2.0f;
			cusps[0] = se::vec2(cos(firstPosition), sin(firstPosition));

			//Take record of min/max cusp values so that it can be streched to match the desired width/height
			float	minX = cusps[0].x,
				minY = cusps[0].y,
				maxX = cusps[0].x,
				maxY = cusps[0].y;
			for (int i = 1; i < shape; i++)
			{
				//Set position
				cusps[i] = se::vec2(
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

			//Assign cusps quantity to given size_t reference
			numCusps = shape;
			return cusps;
		}
		else
		{
			//switch (shape)
			//{//Predetermine shapes
			//default: break;
			//}
			numCusps = 0;
			return nullptr;
		}
	}

	float getTriangleArea(const se::vec2& v0, const se::vec2& v1, const se::vec2& v2)
	{
		return abs(((v1.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (v1.y - v0.y)) / 2.0f);
	}

	se::vec2 getTriangleCircumcircleCenter(const se::vec2& A, const se::vec2& B, const se::vec2& C)
	{/* https://www.easycalculation.com/analytical/circumcenter-triangle.php */
		const se::vec2 midAB = (A + B) * 0.5f;
		const se::vec2 midBC = (B + C) * 0.5f;
		//? const se::vec2 midCA = (C + A) * 0.5f;
		const float slopeAB = (B.y - A.y) / (B.x - A.x);
		const float slopeBC = (C.y - B.y) / (C.x - B.x);
		const float slopeCA = (A.y - C.y) / (A.x - C.x);
		const float perpendicularBisectorSlopeAB = -1.0f / slopeAB;
		const float perpendicularBisectorSlopeBC = -1.0f / slopeBC;
		//? const float perpendicularBisectorSlopeCA = -1.0f / slopeCA;

		se::vec2 center;
		center.x = (-perpendicularBisectorSlopeAB * midAB.x + midAB.y + perpendicularBisectorSlopeBC * midBC.x - midBC.y) / (perpendicularBisectorSlopeBC - perpendicularBisectorSlopeAB);
		center.y = perpendicularBisectorSlopeBC * center.x - perpendicularBisectorSlopeBC * midBC.x + midBC.y;
		return center;
	}
}