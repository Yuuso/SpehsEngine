#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"


namespace se
{
	namespace graphics
	{

		Shape::Shape(const unsigned _numVertices)
			: Primitive()
		{
			recreate(_numVertices);
		}
		Shape::~Shape()
		{
		}

		void Shape::recreate(const unsigned _numVertices)
		{
			// Generate an equilateral convex polygon with the given amount of vertices

			se_assert(_numVertices >= 3);
			vertices.clear();
			indices.clear();

			switch (_numVertices)
			{
				case 3: name = "triangle"; break;
				case 4: name = "square"; break;
				case 5: name = "pentagon"; break;
				case 6: name = "hexagon"; break;
				case 7: name = "heptagon"; break;
				case 8: name = "octagon"; break;
				case 9: name = "nonegon"; break;
				case 10: name = "decagon"; break;
				default: name = "circle";  break;
			}

			// Position
			{
				vertices.resize(_numVertices);

				// Initial rotation is set so that the "lowest" (bottom) line is horizontal
				// firstPosition adjusts initial the rotation for even numbered polygons
				float firstPosition;
				if (_numVertices % 2)
				{
					firstPosition = 0;
				}
				else
				{
					firstPosition = (se::TWO_PI<float> / float(_numVertices)) / 2.0f;
				}

				vertices[0].position = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

				float minX = vertices[0].position.x;
				float minZ = vertices[0].position.z;
				float maxX = vertices[0].position.x;
				float maxZ = vertices[0].position.z;

				for (unsigned i = 1; i < _numVertices; i++)
				{
					vertices[i].position = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / _numVertices)), 0.0f, sin(firstPosition + i * (se::TWO_PI<float> / _numVertices)));

					if		(vertices[i].position.x > maxX)		maxX = vertices[i].position.x;
					else if (vertices[i].position.x < minX)		minX = vertices[i].position.x;
					if		(vertices[i].position.z > maxZ)		maxZ = vertices[i].position.z;
					else if (vertices[i].position.z < minZ)		minZ = vertices[i].position.z;
				}

				const float width = abs(maxX) + abs(minX);
				const float height = abs(maxZ) + abs(minZ);
				for (unsigned i = 0; i < _numVertices; i++)
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
	}
}
