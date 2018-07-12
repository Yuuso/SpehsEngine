#pragma once
#include "SpehsEngine/Core/Vector.h"

namespace se
{
	class Vertex;

	/**Returns the magnitude of vector*/
	extern inline float magnitude(const se::vec2& vector);

	/**Returns distance from origin to destination*/
	extern inline float distance(const se::vec2& origin, const se::vec2& destination);

	/**Returns angle from origin to destination*/
	extern inline float getAcuteAngle(const se::vec2& vector1, const se::vec2& vector2);

	/**Returns angle from origin to destination*/
	extern inline float getAngle(const se::vec2& origin, const se::vec2& destination);
	
	/**Returns angle from 0,0 to destination*/
	extern inline float getAngle(const se::vec2& destination);

	/**Returns angle from angle1 to angle2. Return angle and argument ranges must be within [0.0, TWO_PI]*/
	extern inline float getAngle(const float angle1, const float angle2);

	/**Returns true if two angles (range ]0, TWO_PI[) are parallel with eachother. If angle between angles is more than the tolerance, returns false.*/
	extern inline bool anglesParallel(float angle1, float angle2, const float parallelToleranceRad);

	extern inline se::vec3 getRotation(const se::vec3& _a, const se::vec3& _b);

	/**Rotates point around specified axis point*/
	extern inline void rotateAroundAxis(se::vec2& point, const se::vec2& axis, float rotation);
	extern inline void rotateAroundAxis(se::vec2& point, const float axisX, const float axisY, float rotation);

	/**Rotates point around coordinate system origin*/
	extern inline void rotateAroundOrigin(se::vec2& point, float rotation);

	/**Returns area of the shape formed by a number of cusps*/
	extern inline float getArea(se::vec2* cusps, const size_t numCusps);

	/**Returns radius from 0,0 to the farthest away cusp in the array*/
	extern inline float getRadius(se::vec2* cusps, const size_t numCusps);

	/*Returns center of a convex polygon*/
	extern inline se::vec2 getCenter(se::Vertex* cusps, const size_t numCusps);
	extern inline se::vec2 getCenter(se::vec2* cusps, const size_t numCusps);

	extern inline float cross2(const se::vec2& _vec1, const se::vec2& _vec2);
	extern inline se::vec2 cross2(const float& _scalar, const se::vec2& _vec);
	extern inline se::vec2 cross2(const se::vec2& _vec, const float& _scalar);

	extern float pointToLineDistance(const se::vec2& _begin, const se::vec2& _end, const se::vec2& _point);

	/**Returns a generated position array. Assigns length of array into numCusps*/
	se::vec2* generateCusps(size_t& numCusps, int shape, float width, float height);

	/*Get triangle area*/
	float getTriangleArea(const se::vec2& v0, const se::vec2& v1, const se::vec2& v2);

	/*Get triangle circumcircle center*/
	se::vec2 getTriangleCircumcircleCenter(const se::vec2& A, const se::vec2& B, const se::vec2& C);
}