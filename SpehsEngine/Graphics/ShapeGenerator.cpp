#include "stdafx.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"

#include "SpehsEngine/Math/Mathematics.h"


namespace se
{
	namespace graphics
	{
		void ShapeGenerator::clear()
		{
			vertexBufferCaches.clear();
			indexBufferCaches.clear();
		}

		std::shared_ptr<VertexBuffer> ShapeGenerator::getVertexBuffer(const size_t _numVertices)
		{
			if (_numVertices < 3)
			{
				log::error("Cannot generate a shape vertex buffer with less than 3 vertices!");
				return nullptr;
			}

			ShapeType shapeType = static_cast<ShapeType>(_numVertices);
			unsigned int resolution = 0;
			if (_numVertices >= static_cast<size_t>(ShapeType::Circle))
			{
				shapeType = ShapeType::Circle;
				resolution = static_cast<unsigned int>(_numVertices);
			}

			auto it = std::find_if(vertexBufferCaches.begin(), vertexBufferCaches.end(),
								   [&](const std::unique_ptr<VertexBufferCache>& _vbc)
								   {
									   return _vbc->type == shapeType && _vbc->resolution == resolution;
								   });
			if (it != vertexBufferCaches.end())
				return it->get()->buffer;


			// Generate an equilateral convex polygon with the given amount of vertices

			std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
			VertexBuffer& newVertices = *newVertexBuffer.get();
			using namespace VertexAttribute;
			newVertices.setAttributes(Position
									  | Normal
									  | TexCoord0);

			newVertices.resize(_numVertices);

			// Position
			{
				// Initial rotation is set so that the "lowest" (bottom) line is horizontal
				// firstPosition adjusts initial the rotation for even numbered polygons
				float firstPosition;
				if (_numVertices % 2)
				{
					firstPosition = 0;
				}
				else
				{
					firstPosition = (se::TWO_PI<float> / static_cast<float>(_numVertices)) / 2.0f;
				}

				glm::vec3& firstPositionVertex = newVertices.get<Position>(0);
				firstPositionVertex = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

				float minX = firstPositionVertex.x;
				float minZ = firstPositionVertex.z;
				float maxX = firstPositionVertex.x;
				float maxZ = firstPositionVertex.z;

				for (size_t i = 1; i < _numVertices; i++)
				{
					glm::vec3& positionVertex = newVertices.get<Position>(i);
					positionVertex = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / static_cast<float>(_numVertices))),
											   0.0f,
											   sin(firstPosition + i * (se::TWO_PI<float> / static_cast<float>(_numVertices))));
					maxX = glm::max(maxX, positionVertex.x);
					minX = glm::min(minX, positionVertex.x);
					maxZ = glm::max(maxZ, positionVertex.z);
					minZ = glm::min(minZ, positionVertex.z);
				}

				const float width = abs(maxX) + abs(minX);
				const float height = abs(maxZ) + abs(minZ);
				for (size_t i = 0; i < _numVertices; i++)
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

			vertexBufferCaches.push_back(std::make_unique<VertexBufferCache>(shapeType, resolution, newVertexBuffer));
			return newVertexBuffer;
		}
		std::shared_ptr<VertexBuffer> ShapeGenerator::getVertexBuffer(const ShapeType _shapeType, const unsigned int _resolution)
		{
			if (_shapeType >= ShapeType::Triangle && _shapeType <= ShapeType::Circle)
			{
				size_t numVertices = static_cast<size_t>(_shapeType);
				if (_shapeType == ShapeType::Circle && _resolution >= static_cast<unsigned int>(numVertices))
				{
					numVertices = static_cast<size_t>(_resolution);
				}
				else if (_resolution > 0)
				{
					if (_shapeType != ShapeType::Circle)
					{
						log::warning("Shape: Non-circle 2d shapes don't have a resolution!");
					}
					else
					{
						log::warning("Shape: Minimum circle resolution is " + std::to_string(numVertices) + "!");
					}
				}
				return getVertexBuffer(numVertices);
			}
			else if (_shapeType == ShapeType::Cube)
			{
				constexpr unsigned int cubeResolution = 0;
				if (_resolution != cubeResolution)
					log::warning("Shape: Cube doesn't have a resolution!");

				auto it = std::find_if(vertexBufferCaches.begin(), vertexBufferCaches.end(),
									   [&](const std::unique_ptr<VertexBufferCache>& _vbc)
									   {
										   return _vbc->type == _shapeType && _vbc->resolution == cubeResolution;
									   });
				if (it != vertexBufferCaches.end())
					return it->get()->buffer;


				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				newVertices.setAttributes(Position
										  | Normal
										  | TexCoord0);

				const size_t numVertices = 24;
				newVertices.resize(numVertices);
				size_t currentVertex = 0;

#pragma region cube_vertices
				// Y+ face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// Y- face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, -1.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// X+ face
				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// X- face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(-1.0f, 0.0f, 0.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// Z+ face
				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, 1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f);
				currentVertex++;

				// Z- face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f);
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<Normal>(currentVertex) = glm::vec3(0.0f, 0.0f, -1.0f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f);
				currentVertex++;
#pragma endregion

				vertexBufferCaches.push_back(std::make_unique<VertexBufferCache>(_shapeType, cubeResolution, newVertexBuffer));
				return newVertexBuffer;
			}
			else if (_shapeType == ShapeType::Sphere)
			{
				constexpr unsigned int minRes = 8;
				if (_resolution > 0 && _resolution < minRes)
					log::warning("Shape: Minimum sphere resolution is " + std::to_string(minRes) + "!");
				const unsigned int resolution = _resolution >= minRes ? _resolution : 28;

				auto it = std::find_if(vertexBufferCaches.begin(), vertexBufferCaches.end(),
									   [&](const std::unique_ptr<VertexBufferCache>& _vbc)
									   {
										   return _vbc->type == _shapeType && _vbc->resolution == resolution;
									   });
				if (it != vertexBufferCaches.end())
					return it->get()->buffer;


				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				newVertices.setAttributes(Position
										  | Normal
										  | TexCoord0);

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

						newVertices.get<Position>(currentVertex) = glm::vec3(radius * sinf(lon) * cosf(lat), radius * cosf(lon), radius * sinf(lon) * sinf(lat));
						newVertices.get<Normal>(currentVertex) = glm::normalize(newVertices.get<Position>(currentVertex));
						newVertices.get<TexCoord0>(currentVertex) = glm::vec2(i / resolution, j / resolution);
						currentVertex++;
					}
				}

				vertexBufferCaches.push_back(std::make_unique<VertexBufferCache>(_shapeType, resolution, newVertexBuffer));
				return newVertexBuffer;
			}
			else
			{
				log::error("Invalid shape type!");
			}
			return nullptr;
		}
		std::shared_ptr<IndexBuffer> ShapeGenerator::getIndexBuffer(const size_t _numVertices, const ShapeType _shapeType, const PrimitiveType _primitiveType)
		{
			if (_numVertices == 0)
			{
				log::error("Cannot generate index buffer for 0 vertices!");
				return nullptr;
			}

			auto it = std::find_if(indexBufferCaches.begin(), indexBufferCaches.end(),
								   [&](const std::unique_ptr<IndexBufferCache>& _ibc)
								   {
									   return _ibc->numVertices == _numVertices && _ibc->shapeType == _shapeType && _ibc->primitiveType == _primitiveType;
								   });
			if (it != indexBufferCaches.end())
				return it->get()->buffer;


			std::shared_ptr<IndexBuffer> newIndexBuffer = std::make_shared<IndexBuffer>();
			IndexBuffer& newIndices = *newIndexBuffer.get();

			// NOTE: Assuming CCW front face

			switch (_primitiveType)
			{
				default:
					log::error("Unknown primitive type!");
					return nullptr;
				case se::graphics::Triangles:
				{
					if (_shapeType == ShapeType::Cube)
					{
						newIndices.resize((_numVertices - 2) * 3);
						se_assert(_numVertices % 4 == 0);
						const size_t numQuads = _numVertices / 4;
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
					else if (_shapeType == ShapeType::Sphere)
					{
						const IndexType res = static_cast<IndexType>(sqrt(_numVertices));
						size_t currentIndex = 0;
						const IndexType lastVertex = static_cast<IndexType>(_numVertices) - res - 1;
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
						newIndices.resize((_numVertices - 2) * 3);
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
					newIndices.resize(_numVertices * 2);
					for (size_t i = 0; i < newIndices.size(); )
					{
						newIndices[i++] = index;
						newIndices[i++] = ++index % _numVertices;
					}
					break;
				}
				case se::graphics::Points:
				{
					newIndices.resize(_numVertices);
					for (size_t i = 0; i < newIndices.size(); i++)
						newIndices[i] = static_cast<IndexType>(i);
					break;
				}
			}

			indexBufferCaches.push_back(std::make_unique<IndexBufferCache>(_numVertices, _shapeType, _primitiveType, newIndexBuffer));
			return newIndexBuffer;
		}
	}
}
