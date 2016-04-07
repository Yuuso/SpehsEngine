
#include "Shapes.h"
#include "Vertex.h"
#include "Exceptions.h"

#include <math.h>

#define PI 3.14159265358979f

namespace spehs
{
	std::vector<Vertex> getShapeVertexArray(Shape shape)
	{
		switch (shape)
		{
		case RIGHTANGLED_TRIANGLE:
		{
			std::vector<Vertex> vertexArray;
			vertexArray.resize(3);
			vertexArray[0].position.setPosition(-0.5f, -0.5f);
			vertexArray[1].position.setPosition(0.5f, -0.5f);
			vertexArray[2].position.setPosition(-0.5f, 0.5f);
			return vertexArray;
		}
		case ISOSCELES_TRIANGLE:
		{
			std::vector<Vertex> vertexArray;
			vertexArray.resize(3);
			vertexArray[0].position.setPosition(0.0f, 0.5f);
			vertexArray[1].position.setPosition(cos((7.0f / 6.0f) * PI) * 0.5f, sin((7.0f / 6.0f) * PI) * 0.5f);
			vertexArray[2].position.setPosition(cos((11.0f / 6.0f) * PI) * 0.5f, sin((11.0f / 6.0f) * PI) * 0.5f);
			return vertexArray;
		}
		case BUTTON:
		{
			std::vector<Vertex> vertexArray;
			vertexArray.resize(4);
			vertexArray[0].position.setPosition(0.0f, 0.0f);
			vertexArray[1].position.setPosition(1.0f, 0.0f);
			vertexArray[2].position.setPosition(1.0f, 1.0f);
			vertexArray[3].position.setPosition(0.0f, 1.0f);
			return vertexArray;
		}
		default:
			break;
		}
		exceptions::fatalError("Invalid shape!");
		std::vector<Vertex> errorVector;
		return errorVector;
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