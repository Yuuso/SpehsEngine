#include "stdafx.h"
#include "SpehsEngine/Graphics/Shape.h"

#include "SpehsEngine/Core/Log.h"
#include "SpehsEngine/Core/ColorUtilityFunctions.h"
#include "SpehsEngine/Math/Mathematics.h"


namespace se
{
	namespace graphics
	{
		Shape::Shape(const unsigned int _numVertices)
			: Primitive()
		{
			generate(_numVertices);
		}
		Shape::Shape(const ShapeType _type, const unsigned int _resolution)
			: Primitive()
		{
			generate(_type, _resolution);
			if (_type == ShapeType::Cube || _type == ShapeType::Sphere)
				enableRenderFlag(RenderFlag::CullBackFace);
		}
		Shape::~Shape()
		{
		}

		void Shape::generate(const unsigned int _numVertices)
		{
			// Generate an equilateral convex polygon with the given amount of vertices

			std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
			VertexBuffer& newVertices = *newVertexBuffer.get();
			using namespace VertexAttribute;
			newVertices.setAttributes(Position
									  | Normal
									  | TexCoord0);

			se_assert(_numVertices >= 3);
			const size_t numVertices = std::max(3u, _numVertices);
			newVertices.resize(numVertices);

			switch (_numVertices)
			{
				case 3:		setName("triangle");	break;
				case 4:		setName("square");		break;
				case 5:		setName("pentagon");	break;
				case 6:		setName("hexagon");		break;
				case 7:		setName("heptagon");	break;
				case 8:		setName("octagon");		break;
				case 9:		setName("nonegon");		break;
				case 0:		setName("decagon");		break;
				default:	setName("circle");		break;
			}

			// Position
			{
				// Initial rotation is set so that the "lowest" (bottom) line is horizontal
				// firstPosition adjusts initial the rotation for even numbered polygons
				float firstPosition;
				if (numVertices % 2)
				{
					firstPosition = 0;
				}
				else
				{
					firstPosition = (se::TWO_PI<float> / float(numVertices)) / 2.0f;
				}

				glm::vec3& firstPositionVertex = newVertices.get<Position>(0);
				firstPositionVertex = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

				float minX = firstPositionVertex.x;
				float minZ = firstPositionVertex.z;
				float maxX = firstPositionVertex.x;
				float maxZ = firstPositionVertex.z;

				for (unsigned i = 1; i < numVertices; i++)
				{
					glm::vec3& positionVertex = newVertices.get<Position>(i);
					positionVertex = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / numVertices)), 0.0f, sin(firstPosition + i * (se::TWO_PI<float> / numVertices)));
					maxX = glm::max(maxX, positionVertex.x);
					minX = glm::min(minX, positionVertex.x);
					maxZ = glm::max(maxZ, positionVertex.z);
					minZ = glm::min(minZ, positionVertex.z);
				}

				const float width = abs(maxX) + abs(minX);
				const float height = abs(maxZ) + abs(minZ);
				for (unsigned i = 0; i < numVertices; i++)
				{
					glm::vec3& positionVertex = newVertices.get<Position>(i);
					positionVertex.x /= width;
					positionVertex.z /= height;
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
					glm::vec2& texCoord = newVertices.get<TexCoord0>(i);
					const glm::vec3& positionVertex = newVertices.get<Position>(i);
					texCoord.x = (positionVertex.x + 0.5f);
					texCoord.y = (-positionVertex.z + 0.5f);
				}
			}

			Primitive::setVertices(newVertexBuffer);

			// Indices
			{
				generateIndices();
			}
		}
		void Shape::generate(const ShapeType _type, const unsigned int _resolution)
		{
			type = _type;

			if (type >= ShapeType::Triangle && type <= ShapeType::Circle)
			{
				unsigned int numVertices = static_cast<unsigned int>(type);
				if (type == ShapeType::Circle && _resolution >= numVertices)
				{
					numVertices = _resolution;
				}
				else if (_resolution > 0)
				{
					if (type != ShapeType::Circle)
					{
						log::warning("Shape: Non-circle 2d shapes don't have a resolution!");
					}
					else
					{
						log::warning("Shape: Minimum circle resolution is " + std::to_string(numVertices) + "!");
					}
				}
				generate(numVertices);
			}
			else if (type == ShapeType::Cube)
			{
				setName("cube");
				if (_resolution > 0)
					log::warning("Shape: Cube doesn't have a resolution!");

				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				newVertices.setAttributes(Position
										  | Normal
										  | TexCoord0);

				const size_t numVertices = 24;
				newVertices.resize(numVertices);
				size_t currentVertex = 0;

				// Y+ face
				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// Y- face
				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// X+ face
				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// X- face
				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// Z+ face
				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// Z- face
				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>	(currentVertex)	= glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>		(currentVertex)	= glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(0.0f, 1.0f);
				currentVertex++;

				Primitive::setVertices(newVertexBuffer);
				generateIndices();
			}
			else if (type == ShapeType::Sphere)
			{
				setName("sphere");

				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				newVertices.setAttributes(Position
										  | Normal
										  | TexCoord0);

				constexpr unsigned int minRes = 8;
				if (_resolution > 0 && _resolution < minRes)
					log::warning("Shape: Minimum sphere resolution is " + std::to_string(minRes) + "!");
				const size_t resolution = _resolution >= minRes ? _resolution : 28;
				const size_t numVertices = resolution * resolution;
				newVertices.resize(numVertices);
				size_t currentVertex = 0;

				// NOTE:
				// - Pole vertices could be optimized to just 1 vertex
				// - latitude resolution is actually resolution - 1

				const float radius = 0.5f;
				const float res = static_cast<float>(resolution - 1);
				for (float i = 0.0f; i <= res; i += 1.0f)
				{
					const float lat = se::lerp(0.0f, TWO_PI<float>, i / res);
					for (float j = 0.0f; j <= res; j += 1.0f)
					{
						const float lon = se::lerp(0.0f, PI<float>, j / res);

						newVertices.get<Position>	(currentVertex)	= glm::vec3(radius * sinf(lon) * cosf(lat), radius * cosf(lon), radius * sinf(lon) * sinf(lat));
						newVertices.get<Normal>		(currentVertex)	= glm::normalize(newVertices.get<Position>(currentVertex));
						newVertices.get<TexCoord0>	(currentVertex)	= glm::vec2(i / resolution, j / resolution);
						currentVertex++;
					}
				}

				Primitive::setVertices(newVertexBuffer);
				generateIndices();
			}
			else
			{
				log::error("Invalid shape type!");
			}
		}

		void Shape::setPrimitiveType(const PrimitiveType _primitiveType)
		{
			Primitive::setPrimitiveType(_primitiveType);
			generateIndices();
		}
		void Shape::setVertices(std::shared_ptr<VertexBuffer> _vertices)
		{
			se_assert_m(false, "Cannot set vertices for Shape primitive!");
		}
		void Shape::setIndices(std::shared_ptr<IndexBuffer>)
		{
			se_assert_m(false, "Cannot set indices for Shape primitive!");
		}

		void Shape::generateIndices()
		{
			if (!getVertices())
				return;

			const size_t numVertices = getVertices()->size();
			std::shared_ptr<IndexBuffer> newIndexBuffer = std::make_shared<IndexBuffer>();
			IndexBuffer& newIndices = *newIndexBuffer.get();

			// NOTE: Assuming CCW front face

			if (numVertices > 0)
			{
				switch (getPrimitiveType())
				{
					default:
						se_assert_m(false, "Unknown primitive type!");
						[[fallthrough]];
					case se::graphics::Triangles:
					{
						if (type == ShapeType::Cube)
						{
							newIndices.resize((numVertices - 2) * 3);
							se_assert(numVertices % 4 == 0);
							const size_t numQuads = numVertices / 4;
							size_t currentIndex = 0;
							for (size_t i = 0; i < numQuads; i++)
							{
								const IndexType currentVertex = static_cast<IndexType>(i * 4);
								IndexType index = 1;
								newIndices[currentIndex++] = currentVertex;
								newIndices[currentIndex++] = currentVertex + index;
								newIndices[currentIndex++] = currentVertex + (++index);
								newIndices[currentIndex++] = currentVertex;
								newIndices[currentIndex++] = currentVertex + index;
								newIndices[currentIndex++] = currentVertex + (++index);
							}
						}
						else if (type == ShapeType::Sphere)
						{
							const IndexType res = static_cast<IndexType>(sqrt(numVertices));
							size_t currentIndex = 0;
							const IndexType lastVertex = static_cast<IndexType>(numVertices) - res - 1;
							newIndices.resize(lastVertex * 6);
							for (IndexType vertex = 0; vertex < lastVertex; vertex++)
							{
								newIndices[currentIndex++] = vertex;
								newIndices[currentIndex++] = vertex + 1;
								newIndices[currentIndex++] = vertex + res;
								newIndices[currentIndex++] = vertex + res;
								newIndices[currentIndex++] = vertex + 1;
								newIndices[currentIndex++] = vertex + res + 1;
							}
						}
						else
						{
							newIndices.resize((numVertices - 2) * 3);
							IndexType index = 1;
							for (size_t i = 0; i < newIndices.size();)
							{
								newIndices[i++] = 0;
								newIndices[i++] = index;
								newIndices[i++] = ++index;
							}
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
							newIndices[i] = static_cast<IndexType>(i);
						break;
					}
				}
			}
			Primitive::setIndices(newIndexBuffer);
		}
	}
}
