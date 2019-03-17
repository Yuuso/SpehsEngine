#include "stdafx.h"

#include "SpehsEngine/Core/Shapes.h"
#include "SpehsEngine/Core/Vertex.h"

#include <math.h>


namespace se
{
	std::vector<Vertex> getShapeVertexArray(Shape shape)
	{
		std::vector<Vertex> vertexArray;
		switch (shape)
		{
		case RIGHTANGLED_TRIANGLE:
			vertexArray.resize(3);
			vertexArray[0].position = glm::vec2(-0.5f, -0.5f);
			vertexArray[1].position = glm::vec2(0.5f, -0.5f);
			vertexArray[2].position = glm::vec2(-0.5f, 0.5f);
			break;
		case ISOSCELES_TRIANGLE:
			vertexArray.resize(3);
			vertexArray[0].position = glm::vec2(0.0f, 0.5f);
			vertexArray[1].position = glm::vec2(cos((7.0f / 6.0f) * PI<float>) * 0.5f, sin((7.0f / 6.0f) * PI<float>) * 0.5f);
			vertexArray[2].position = glm::vec2(cos((11.0f / 6.0f) * PI<float>) * 0.5f, sin((11.0f / 6.0f) * PI<float>) * 0.5f);
			break;
		case BUTTON:
			vertexArray.resize(4);
			vertexArray[0].position = glm::vec2(0.0f, 0.0f);
			vertexArray[1].position = glm::vec2(1.0f, 0.0f);
			vertexArray[2].position = glm::vec2(1.0f, 1.0f);
			vertexArray[3].position = glm::vec2(0.0f, 1.0f);
			break;
		default:
			log::error("Invalid shape!");
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