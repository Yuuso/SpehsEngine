#include "Geometry.h"
#include <math.h>


namespace SpehsEngine
{
	float magnitude(glm::vec2& vec)
	{
		return sqrt(vec.x*vec.x + vec.y*vec.y);
	}
	float distance(glm::vec2& origin, glm::vec2& destination)
	{
		return magnitude(origin - destination);
	}
	float getAngle(glm::vec2& origin, glm::vec2& destination)
	{
		glm::vec2 vec = destination - origin;
		float pointDirection = atan2(vec.y, vec.x);
		if (vec.y < 0)
			pointDirection += TWO_PI;
		return pointDirection;
	}
	float getAngle(glm::vec2& destination)
	{
		float pointDirection = atan2(destination.y, destination.x);
		if (destination.y < 0)
			pointDirection += TWO_PI;
		return pointDirection;
	}
	float getAngle(float angle1, float angle2)
	{
		float angle = angle2 - angle1;
		if (angle < 0)
			angle += TWO_PI;
		return angle;
	}
}