#pragma once
#include <glm/vec2.hpp>
#define GLM_FORCE_RADIANS
#define PI 3.14159265f
#define HALF_PI 1.57079632679f
#define TWO_PI 6.28318530718f

namespace spehs
{
	class Position;

	/**Returns the magnitude of vector*/
	extern inline float magnitude(glm::vec2& vector);
	extern inline float magnitude(spehs::Position& position);

	/**Returns distance from origin to destination*/
	extern inline float distance(glm::vec2& origin, glm::vec2& destination);

	/**Returns angle from origin to destination*/
	extern inline float getAngle(glm::vec2& origin, glm::vec2& destination);
	
	/**Returns angle from 0,0 to destination*/
	extern inline float getAngle(glm::vec2& destination);

	/**Returns angle from angle1 to angle2*/
	extern inline float getAngle(float angle1, float angle2);

	/**Returns area of the shape formed by a number of cusps*/
	extern inline float getArea(spehs::Position* cusps, unsigned numCusps);

	/**Returns redius from 0,0 to the farthest away cusp in the array*/
	extern inline float getRadius(spehs::Position* cusps, unsigned numCusps);

	/**Returns a generated position array. Assigns length of array into numCusps*/
	spehs::Position* generateCusps(unsigned& numCusps, int shape, float width, float height);
}