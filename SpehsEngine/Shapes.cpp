
#include "Shapes.h"
#include "Vertex.h"
#include "Exceptions.h"

#include <math.h>

#define PI 3.14159265358979f

namespace spehs
{
	std::vector<Vertex> getShapeVertexArray(Shape shape)
	{
		std::vector<Vertex> vertexArray;
		switch (shape)
		{
		case RIGHTANGLED_TRIANGLE:
			vertexArray.resize(3);
			vertexArray[0].position.setPosition(-0.5f, -0.5f);
			vertexArray[1].position.setPosition(0.5f, -0.5f);
			vertexArray[2].position.setPosition(-0.5f, 0.5f);
			break;
		case ISOSCELES_TRIANGLE:
			vertexArray.resize(3);
			vertexArray[0].position.setPosition(0.0f, 0.5f);
			vertexArray[1].position.setPosition(cos((7.0f / 6.0f) * PI) * 0.5f, sin((7.0f / 6.0f) * PI) * 0.5f);
			vertexArray[2].position.setPosition(cos((11.0f / 6.0f) * PI) * 0.5f, sin((11.0f / 6.0f) * PI) * 0.5f);
			break;
		case BUTTON:
			vertexArray.resize(4);
			vertexArray[0].position.setPosition(0.0f, 0.0f);
			vertexArray[1].position.setPosition(1.0f, 0.0f);
			vertexArray[2].position.setPosition(1.0f, 1.0f);
			vertexArray[3].position.setPosition(0.0f, 1.0f);
			break;
		default:
			exceptions::fatalError("Invalid shape!");
			break;
		}
		return vertexArray;
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