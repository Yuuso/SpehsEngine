#pragma once
#include "Vector.h"

namespace spehs
{
	class Vertex;

	/**Returns the magnitude of vector*/
	extern inline float magnitude(const spehs::vec2& vector);

	/**Returns distance from origin to destination*/
	extern inline float distance(const spehs::vec2& origin, const spehs::vec2& destination);

	/**Returns angle from origin to destination*/
	extern inline float getAcuteAngle(const spehs::vec2& vector1, const spehs::vec2& vector2);

	/**Returns angle from origin to destination*/
	extern inline float getAngle(const spehs::vec2& origin, const spehs::vec2& destination);
	
	/**Returns angle from 0,0 to destination*/
	extern inline float getAngle(const spehs::vec2& destination);

	/**Returns angle from angle1 to angle2. Return angle and argument ranges must be within [0.0, TWO_PI]*/
	extern inline float getAngle(const float angle1, const float angle2);

	/**Returns true if two angles (range ]0, TWO_PI[) are parallel with eachother. If angle between angles is more than the tolerance, returns false.*/
	extern inline bool anglesParallel(float angle1, float angle2, const float parallelToleranceRad);

	extern inline spehs::vec3 getRotation(const spehs::vec3& _a, const spehs::vec3& _b);

	/**Rotates point around specified axis point*/
	extern inline void rotateAroundAxis(spehs::vec2& point, const spehs::vec2& axis, float rotation);
	extern inline void rotateAroundAxis(spehs::vec2& point, const float axisX, const float axisY, float rotation);

	/**Rotates point around coordinate system origin*/
	extern inline void rotateAroundOrigin(spehs::vec2& point, float rotation);

	/**Returns area of the shape formed by a number of cusps*/
	extern inline float getArea(spehs::vec2* cusps, unsigned numCusps);

	/**Returns radius from 0,0 to the farthest away cusp in the array*/
	extern inline float getRadius(spehs::vec2* cusps, unsigned numCusps);

	/*Returns center of a convex polygon*/
	extern inline spehs::vec2 getCenter(spehs::Vertex* cusps, unsigned numCusps);
	extern inline spehs::vec2 getCenter(spehs::vec2* cusps, unsigned numCusps);

	extern inline float cross2(const spehs::vec2& _vec1, const spehs::vec2& _vec2);
	extern inline spehs::vec2 cross2(const float& _scalar, const spehs::vec2& _vec);
	extern inline spehs::vec2 cross2(const spehs::vec2& _vec, const float& _scalar);

	extern float pointToLineDistance(const spehs::vec2& _begin, const spehs::vec2& _end, const spehs::vec2& _point);

	/**Returns a generated position array. Assigns length of array into numCusps*/
	spehs::vec2* generateCusps(unsigned& numCusps, int shape, float width, float height);

	/*Get triangle area*/
	float getTriangleArea(const spehs::vec2& v0, const spehs::vec2& v1, const spehs::vec2& v2);

	/*Get triangle circumcircle center*/
	spehs::vec2 getTriangleCircumcircleCenter(const spehs::vec2& A, const spehs::vec2& B, const spehs::vec2& C);
}