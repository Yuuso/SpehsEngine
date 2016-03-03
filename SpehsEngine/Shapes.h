#pragma once

#include "Primitive.h"
#include "Vertex.h"

#include <math.h>


#define PI 3.14159265f


//Make all shapes 1.0f x 1.0f by size

namespace spehs
{
	Vertex* getShapeVertexArray(Shape shape) //Deleting is handled by the caller of the function.
	{
		switch (shape)
		{
		case RIGHTANGLED_TRIANGLE:
		{
			Vertex* vertexArray = new Vertex[3];
			vertexArray[0].setPosition(-0.5f, -0.5f);
			vertexArray[1].setPosition(0.5f, -0.5f);
			vertexArray[2].setPosition(-0.5f, 0.5f);
			return vertexArray;
		}
		case ISOSCELES_TRIANGLE:
		{
			Vertex* vertexArray = new Vertex[3];
			vertexArray[0].setPosition(0.0f, 0.5f);
			vertexArray[1].setPosition(cos((7.0f / 6.0f) * PI) * 0.5f, sin((7.0f / 6.0f) * PI) * 0.5f);
			vertexArray[2].setPosition(cos((11.0f / 6.0f) * PI) * 0.5f, sin((11.0f / 6.0f) * PI) * 0.5f);
			return vertexArray;
		}
		case BUTTON:
		{
			Vertex* vertexArray = new Vertex[4];
			vertexArray[0].setPosition(0.0f, 0.0f);
			vertexArray[1].setPosition(1.0f, 0.0f);
			vertexArray[2].setPosition(1.0f, 1.0f);
			vertexArray[3].setPosition(0.0f, 1.0f);
			return vertexArray;
		}
		default:
			break;
		}
		return nullptr;
	}
	
	int getNumVertices(Shape shape)
	{
		switch (shape)
		{
		case RIGHTANGLED_TRIANGLE:
			return 3;
		case ISOSCELES_TRIANGLE:
			return 3;
		case BUTTON:
			return 4;
		default:
			break;
		}
		return 0;
	}
}