#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"


namespace se
{
	namespace graphics
	{

		Shape::Shape(const unsigned int numVertices)
			: Primitive()
		{
			se_assert(numVertices >= 3);

			// Position
			{
				vertices.resize(numVertices);

				// firstPosition adjusts initial the rotation for even numbered polygons
				// Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
				float firstPosition;
				if (numVertices % 2)
				{
					firstPosition = 0;
				}
				else
				{
					firstPosition = (se::TWO_PI<float> / float(numVertices)) / 2.0f;
				}

				vertices[0].position = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

				float minX = vertices[0].position.x;
				float minZ = vertices[0].position.z;
				float maxX = vertices[0].position.x;
				float maxZ = vertices[0].position.z;

				for (unsigned i = 1; i < numVertices; i++)
				{
					vertices[i].position = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / numVertices)), 0.0f, sin(firstPosition + i * (se::TWO_PI<float> / numVertices)));

					if		(vertices[i].position.x > maxX)		maxX = vertices[i].position.x;
					else if (vertices[i].position.x < minX)		minX = vertices[i].position.x;
					if		(vertices[i].position.z > maxZ)		maxZ = vertices[i].position.z;
					else if (vertices[i].position.z < minZ)		minZ = vertices[i].position.z;
				}

				const float width = abs(maxX) + abs(minX);
				const float height = abs(maxZ) + abs(minZ);
				for (unsigned i = 0; i < numVertices; i++)
				{
					vertices[i].position.x /= width;
					vertices[i].position.z /= height;

					vertices[i].position.x *= 10.0f;
					vertices[i].position.z *= 10.0f;
				}
			}

			// Normal
			{
				for (size_t i = 0; i < vertices.size(); i++)
				{
					vertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
				}
			}

			// UV
			{
				for (size_t i = 0; i < vertices.size(); i++)
				{
					vertices[i].uv.x = (vertices[i].position.x + 0.5f);
					vertices[i].uv.y = (-vertices[i].position.y + 0.5f);
				}
			}

			// Indices
			{
				uint16_t currentIndex = 0;
				size_t index = 0;

				indices.resize((vertices.size() / 4) * 6);

				while (index < indices.size())
				{
					for (uint16_t i = 1; i < 3; i++)
					{
						indices[index++] = currentIndex;
						indices[index++] = currentIndex + i;
						indices[index++] = currentIndex + i + 1;
					}
					currentIndex += 4;
				}
			}
		}
		Shape::~Shape()
		{

		}
	}
}
