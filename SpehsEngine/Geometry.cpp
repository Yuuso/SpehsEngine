#include <math.h>
#include "Geometry.h"
#include "Vertex.h"

namespace spehs
{
	float magnitude(glm::vec2& vec)
	{
		return sqrt(vec.x*vec.x + vec.y*vec.y);
	}
	float magnitude(spehs::Position& pos)
	{
		return sqrt(pos.x*pos.x + pos.y*pos.y);
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
	spehs::Position* generateCusps(unsigned& numCusps, int shape, float width, float height)
	{
		if (!shape)
		{
			numCusps = 0;
			return nullptr;
		}
		if (shape > 2)
		{//Isosceles shape

			//Generate number of cusps desired
			spehs::Position* cusps = new spehs::Position[shape];

			//firstPosition adjusts initial the rotation for even numbered polygons
			float firstPosition;

			//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
			if (shape % 2)
				firstPosition = HALF_PI;
			else
				firstPosition = HALF_PI + (TWO_PI / shape) / 2;
			cusps[0].setPosition(cos(firstPosition), sin(firstPosition));

			//Take record of min/max cusp values so that it can be streched to match the desired width/height
			float	minX = cusps[0].x,
					minY = cusps[0].y,
					maxX = cusps[0].x,
					maxY = cusps[0].y;
			for (int i = 1; i < shape; i++)
			{
				//Set position
				cusps[i].setPosition(
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
			float wFactor = 1.0f / (maxX - minX);//Note: Max is positive, min is negative
			float hFactor = 1.0f / (maxY - minY);//Note: Max is positive, min is negative

			//Rescale cusps
			for (int i = 0; i < shape; i++)
			{
				cusps[i].x *= wFactor;
				cusps[i].y *= hFactor;
			}

			//Finally return shape as the cusp quantity
			numCusps = shape;
		}
		else switch (shape)
		{//Predetermine shapes
		default:
			numCusps = 0;
			return nullptr;
		}
	}
	float getArea(spehs::Position* cusps, unsigned numCusps)
	{
		if (numCusps < 3)
			return 0.0f;
		float area = 0.0f;
		int j = numCusps - 1;
		for (int i = 0; i < numCusps; i++)
		{
			area += (cusps[j].x * cusps[i].y) - (cusps[i].x * cusps[j].y);
			j = i;
		}
		return abs((area * 0.5f));
	}
	float getRadius(spehs::Position* cusps, unsigned numCusps)
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
}