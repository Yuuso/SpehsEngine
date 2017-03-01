#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#define GLM_FORCE_RADIANS
#define PI 3.14159265f
#define HALF_PI 1.57079632679f
#define TWO_PI 6.28318530718f

namespace spehs
{
	class Vertex;

	/**Returns the magnitude of vector*/
	extern inline float magnitude(const glm::vec2& vector);

	/**Returns distance from origin to destination*/
	extern inline float distance(const glm::vec2& origin, const glm::vec2& destination);

	/**Returns angle from origin to destination*/
	extern inline float getAcuteAngle(const glm::vec2& vector1, const glm::vec2& vector2);

	/**Returns angle from origin to destination*/
	extern inline float getAngle(const glm::vec2& origin, const glm::vec2& destination);
	
	/**Returns angle from 0,0 to destination*/
	extern inline float getAngle(const glm::vec2& destination);

	/**Returns angle from angle1 to angle2. Return angle and argument ranges must be within [0.0, TWO_PI]*/
	extern inline float getAngle(const float angle1, const float angle2);

	/**Returns true if two angles (range ]0, TWO_PI[) are parallel with eachother. If angle between angles is more than the tolerance, returns false.*/
	extern inline bool anglesParallel(float angle1, float angle2, const float parallelToleranceRad);

	extern inline glm::vec3 getRotation(const glm::vec3& _a, const glm::vec3& _b);

	/**Rotates point around specified axis point*/
	extern inline void rotateAroundAxis(glm::vec2& point, const glm::vec2& axis, float rotation);
	extern inline void rotateAroundAxis(glm::vec2& point, const float axisX, const float axisY, float rotation);

	/**Rotates point around coordinate system origin*/
	extern inline void rotateAroundOrigin(glm::vec2& point, float rotation);

	/**Returns area of the shape formed by a number of cusps*/
	extern inline float getArea(glm::vec2* cusps, unsigned numCusps);

	/**Returns radius from 0,0 to the farthest away cusp in the array*/
	extern inline float getRadius(glm::vec2* cusps, unsigned numCusps);

	/*Returns center of a convex polygon*/
	extern inline glm::vec2 getCenter(spehs::Vertex* cusps, unsigned numCusps);
	extern inline glm::vec2 getCenter(glm::vec2* cusps, unsigned numCusps);

	extern inline float cross2(const glm::vec2& _vec1, const glm::vec2& _vec2);
	extern inline glm::vec2 cross2(const float& _scalar, const glm::vec2& _vec);
	extern inline glm::vec2 cross2(const glm::vec2& _vec, const float& _scalar);

	extern float pointToLineDistance(const glm::vec2 &_begin, const glm::vec2 &_end, const glm::vec2 &_point);

	/**Returns a generated position array. Assigns length of array into numCusps*/
	glm::vec2* generateCusps(unsigned& numCusps, int shape, float width, float height);

	/*Get triangle area*/
	float getTriangleArea(const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2);
}