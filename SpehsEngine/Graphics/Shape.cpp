#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"

#include "SpehsEngine/Core/Log.h"


namespace se
{
	namespace graphics
	{

		Shape::Shape(const unsigned _numVertices)
			: Primitive()
		{
			generate(_numVertices);
		}
		Shape::~Shape()
		{
		}

		void Shape::generate(const unsigned _numVertices)
		{
			// Generate an equilateral convex polygon with the given amount of vertices

			se_assert(_numVertices >= 3);
			std::vector<Vertex> newVertices;

			switch (_numVertices)
			{
				case 3:  setName("triangle"); break;
				case 4:  setName("square");	  break;
				case 5:  setName("pentagon"); break;
				case 6:  setName("hexagon");  break;
				case 7:  setName("heptagon"); break;
				case 8:  setName("octagon");  break;
				case 9:  setName("nonegon");  break;
				case 10: setName("decagon");  break;
				default: setName("circle");   break;
			}

			// Position
			{
				newVertices.resize(_numVertices);

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

				newVertices[0].position = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

				float minX = newVertices[0].position.x;
				float minZ = newVertices[0].position.z;
				float maxX = newVertices[0].position.x;
				float maxZ = newVertices[0].position.z;

				for (unsigned i = 1; i < _numVertices; i++)
				{
					newVertices[i].position = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / _numVertices)), 0.0f, sin(firstPosition + i * (se::TWO_PI<float> / _numVertices)));

					if		(newVertices[i].position.x > maxX)		maxX = newVertices[i].position.x;
					else if (newVertices[i].position.x < minX)		minX = newVertices[i].position.x;
					if		(newVertices[i].position.z > maxZ)		maxZ = newVertices[i].position.z;
					else if (newVertices[i].position.z < minZ)		minZ = newVertices[i].position.z;
				}

				const float width = abs(maxX) + abs(minX);
				const float height = abs(maxZ) + abs(minZ);
				for (unsigned i = 0; i < _numVertices; i++)
				{
					newVertices[i].position.x /= width;
					newVertices[i].position.z /= height;
				}
			}

			// Normal
			{
				for (size_t i = 0; i < newVertices.size(); i++)
				{
					newVertices[i].normal = glm::vec3(0.0f, 1.0f, 0.0f);
				}
			}

			// UV
			{
				for (size_t i = 0; i < newVertices.size(); i++)
				{
					newVertices[i].uv.x = (newVertices[i].position.x + 0.5f);
					newVertices[i].uv.y = (-newVertices[i].position.y + 0.5f);
				}
			}

			Primitive::setVertices(newVertices);

			// Indices
			{
				generateIndices();
			}
		}

		void Shape::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			Primitive::setPrimitiveType(_primitiveType);
			generateIndices();
		}
		void Shape::setVertices(const std::vector<Vertex>& _vertices)
		{
			log::error("setVertices should not be used with shapes!");
		}
		void Shape::setIndices(const std::vector<uint16_t>& _indices)
		{
			log::error("setIndices should not be used with shapes!");
		}

		void Shape::generateIndices()
		{
			const size_t numVertices = getVertices().size();
			std::vector<IndexType> newIndices;

			switch (getPrimitiveType())
			{
				default:
					se_assert_m(false, "Unknown primitive type!");
					[[fallthrough]];
				case se::graphics::Triangles:
				{
					IndexType index = 1;
					newIndices.resize((numVertices - 2) * 3);
					for (size_t i = 0; i < newIndices.size();)
					{
						newIndices[i++] = 0;
						newIndices[i++] = index;
						newIndices[i++] = ++index;
					}
					break;
				}
				case se::graphics::Lines:
				{
					IndexType index = 0;
					newIndices.resize(numVertices * 2);
					for (size_t i = 0; i < newIndices.size(); )
					{
						newIndices[i++] = index;
						newIndices[i++] = ++index % numVertices;
					}
					break;
				}
				case se::graphics::Points:
				{
					newIndices.resize(numVertices);
					for (size_t i = 0; i < newIndices.size(); i++)
						newIndices[i] = (IndexType)i;
					break;
				}
			}
			Primitive::setIndices(newIndices);
		}
	}
}
