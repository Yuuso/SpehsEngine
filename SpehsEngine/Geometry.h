#pragma once
#include <glm/vec2.hpp>
#define PI 3.14159265359f
#define TWO_PI 6.28318530718f

namespace SpehsEngine
{
	/*Returns the magnitude of vector*/
	float magnitude(glm::vec2& vector);

	/*Returns distance from origin to destination*/
	float distance(glm::vec2& origin, glm::vec2& destination);

	/*Returns angle from origin to destination*/
	float getAngle(glm::vec2& origin, glm::vec2& destination);
	
	/*Returns angle from 0,0 to destination*/
	float getAngle(glm::vec2& destination);

	/*Returns angle from angle1 to angle2*/
	float getAngle(float angle1, float angle2);
}