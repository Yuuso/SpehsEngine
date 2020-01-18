#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"


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
			VertexBuffer newVertices;
			newVertices.setAttributes(VertexAttributeFlag::Position
									| VertexAttributeFlag::Normal
									| VertexAttributeFlag::Color0
									| VertexAttributeFlag::TexCoord0);

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

				newVertices.get<Position>(0) = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

				float minX = newVertices.get<Position>(0).x;
				float minZ = newVertices.get<Position>(0).z;
				float maxX = newVertices.get<Position>(0).x;
				float maxZ = newVertices.get<Position>(0).z;

				for (unsigned i = 1; i < _numVertices; i++)
				{
					newVertices.get<Position>(i) = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / _numVertices)), 0.0f, sin(firstPosition + i * (se::TWO_PI<float> / _numVertices)));

					if		(newVertices.get<Position>(i).x > maxX)		maxX = newVertices.get<Position>(i).x;
					else if (newVertices.get<Position>(i).x < minX)		minX = newVertices.get<Position>(i).x;
					if		(newVertices.get<Position>(i).z > maxZ)		maxZ = newVertices.get<Position>(i).z;
					else if (newVertices.get<Position>(i).z < minZ)		minZ = newVertices.get<Position>(i).z;
				}

				const float width = abs(maxX) + abs(minX);
				const float height = abs(maxZ) + abs(minZ);
				for (unsigned i = 0; i < _numVertices; i++)
				{
					newVertices.get<Position>(i).x /= width;
					newVertices.get<Position>(i).z /= height;
				}
			}

			// Normal
			{
				for (size_t i = 0; i < newVertices.size(); i++)
				{
					newVertices.get<Normal>(i) = glm::vec3(0.0f, 1.0f, 0.0f);
				}
			}

			// UV
			{
				for (size_t i = 0; i < newVertices.size(); i++)
				{
					newVertices.get<TexCoord0>(i).x = (newVertices.get<TexCoord0>(i).x + 0.5f);
					newVertices.get<TexCoord0>(i).y = (-newVertices.get<TexCoord0>(i).y + 0.5f);
				}
			}

			// Color
			{
				const Color vertexColor = randomBrightColor();
				for (size_t i = 0; i < newVertices.size(); i++)
					newVertices.get<Color0>(i) = vertexColor;
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
		void Shape::setVertices(const VertexBuffer& _vertices)
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
