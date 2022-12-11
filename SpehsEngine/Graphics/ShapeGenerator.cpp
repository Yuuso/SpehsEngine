#include "stdafx.h"
#include "SpehsEngine/Graphics/ShapeGenerator.h"

#include "SpehsEngine/Graphics/IndexBuffer.h"
#include "SpehsEngine/Graphics/VertexBuffer.h"
#include "SpehsEngine/Math/Mathematics.h"


namespace se
{
	namespace graphics
	{
		constexpr unsigned int defaultCircleResolution = 28;

		void ShapeGenerator::clear()
		{
			vertexBufferCaches.clear();
			indexBufferCaches.clear();
		}

		std::shared_ptr<VertexBuffer> ShapeGenerator::getVertexBuffer(const ShapeType _shapeType, const ShapeParameters _shapeParams)
		{
			if (_shapeType >= ShapeType::Triangle && _shapeType <= ShapeType::Circle)
			{
				const ShapeType shapeType = _shapeType;
				size_t numVertices = 0;

				if (shapeType == ShapeType::Circle)
				{
					if (_shapeParams.resolution == 0)
					{
						numVertices = defaultCircleResolution;
					}
					else
					{
						constexpr unsigned int minRes = static_cast<size_t>(ShapeType::Circle);
						if (_shapeParams.resolution < minRes)
						{
							log::warning("Shape: Minimum circle resolution is " + std::to_string(minRes) + "!");
							numVertices = minRes;
						}
						else
						{
							numVertices = _shapeParams.resolution;
						}
					}
				}
				else
				{
					if (_shapeParams.resolution > 0)
						log::warning("Shape: Non-circle flat shapes don't have a resolution!");
					numVertices = static_cast<size_t>(shapeType);
				}

				if (numVertices < 3)
				{
					log::error("Cannot generate a shape vertex buffer with less than 3 vertices!");
					return nullptr;
				}

				auto it = std::find_if(vertexBufferCaches.begin(), vertexBufferCaches.end(),
									   [&](const std::unique_ptr<VertexBufferCache>& _vbc)
									   {
										   return _vbc->type == shapeType && _vbc->params == _shapeParams;
									   });
				if (it != vertexBufferCaches.end())
					return it->get()->buffer;


				// Generate an equilateral convex polygon with the given amount of vertices

				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				VertexAttributeFlagsType attributes = Position | TexCoord0;
				if (_shapeParams.generateNormals)
					enableBit(attributes, Normal);
				if (_shapeParams.generateTangents)
				{
					enableBit(attributes, Tangent);
					enableBit(attributes, Bitangent);
				}
				newVertices.setAttributes(attributes);

				newVertices.resize(numVertices);

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
						firstPosition = (se::TWO_PI<float> / static_cast<float>(numVertices)) / 2.0f;
					}

					glm::vec3& firstPositionVertex = newVertices.get<Position>(0);
					firstPositionVertex = glm::vec3(cos(firstPosition), 0.0f, sin(firstPosition));

					float minX = firstPositionVertex.x;
					float minZ = firstPositionVertex.z;
					float maxX = firstPositionVertex.x;
					float maxZ = firstPositionVertex.z;

					for (size_t i = 1; i < numVertices; i++)
					{
						glm::vec3& positionVertex = newVertices.get<Position>(i);
						positionVertex = glm::vec3(cos(firstPosition + i * (se::TWO_PI<float> / static_cast<float>(numVertices))),
												   0.0f,
												   sin(firstPosition + i * (se::TWO_PI<float> / static_cast<float>(numVertices))));
						maxX = glm::max(maxX, positionVertex.x);
						minX = glm::min(minX, positionVertex.x);
						maxZ = glm::max(maxZ, positionVertex.z);
						minZ = glm::min(minZ, positionVertex.z);
					}

					const float width = abs(maxX) + abs(minX);
					const float height = abs(maxZ) + abs(minZ);
					for (size_t i = 0; i < numVertices; i++)
					{
						glm::vec3& positionVertex = newVertices.get<Position>(i);
						switch (_shapeParams.orientation)
						{
							case se::graphics::ShapeOrientation::XZ_Plane:
								positionVertex.x /= width;
								positionVertex.y = 0.0f;
								positionVertex.z /= height;
								break;
							case se::graphics::ShapeOrientation::XY_Plane:
								positionVertex.x /= width;
								positionVertex.y = positionVertex.z / height;
								positionVertex.z = 0.0f;
								break;
							default:
								se_assert_m(false, "Unknown ShapeOrientation!");
								break;
						}
					}
				}

				// UV
				{
					for (size_t i = 0; i < newVertices.size(); i++)
					{
						glm::vec2& texCoord = newVertices.get<TexCoord0>(i);
						const glm::vec3& positionVertex = newVertices.get<Position>(i);
						switch (_shapeParams.orientation)
						{
							case se::graphics::ShapeOrientation::XZ_Plane:
								texCoord.x = (positionVertex.x + 0.5f) * _shapeParams.uvScale;
								texCoord.y = (-positionVertex.z + 0.5f) * _shapeParams.uvScale;
								break;
							case se::graphics::ShapeOrientation::XY_Plane:
								texCoord.x = (positionVertex.x + 0.5f) * _shapeParams.uvScale;
								texCoord.y = (-positionVertex.y + 0.5f) * _shapeParams.uvScale;
								break;
							default:
								se_assert_m(false, "Unknown ShapeOrientation!");
								break;
						}
					}
				}

				// Normal
				if (_shapeParams.generateNormals)
				{
					for (size_t i = 0; i < newVertices.size(); i++)
					{
						newVertices.get<Normal>(i) = glm::vec3(0.0f, _shapeParams.invertNormals ? -1.0f : 1.0f, 0.0f);

						// Tangents
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(i) = glm::vec3(1.0f, 0.0f, 0.0f);
							newVertices.get<Bitangent>(i) = glm::cross(newVertices.get<Normal>(i), newVertices.get<Tangent>(i));
						}
					}
				}

				vertexBufferCaches.push_back(std::make_unique<VertexBufferCache>(shapeType, _shapeParams, newVertexBuffer));
				return newVertexBuffer;
			}
			else if (_shapeType == ShapeType::Cube)
			{
				constexpr unsigned int cubeResolution = 0;
				if (_shapeParams.resolution != cubeResolution)
					log::warning("Shape: Cube doesn't have a resolution!");

				auto it = std::find_if(vertexBufferCaches.begin(), vertexBufferCaches.end(),
									   [&](const std::unique_ptr<VertexBufferCache>& _vbc)
									   {
										   return _vbc->type == _shapeType && _vbc->params == _shapeParams;
									   });
				if (it != vertexBufferCaches.end())
					return it->get()->buffer;


				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				VertexAttributeFlagsType attributes = Position | TexCoord0;
				if (_shapeParams.generateNormals)
					enableBit(attributes, Normal);
				if (_shapeParams.generateTangents)
				{
					enableBit(attributes, Tangent);
					enableBit(attributes, Bitangent);
				}
				newVertices.setAttributes(attributes);

				const size_t numVertices = 24;
				newVertices.resize(numVertices);
				size_t currentVertex = 0;

#pragma region cube_vertices
				// Y+ face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				for (size_t vertex = currentVertex - 4; vertex < currentVertex; vertex++)
				{
					if (_shapeParams.generateNormals)
					{
						newVertices.get<Normal>(vertex) = glm::vec3(0.0f, _shapeParams.invertNormals ? -1.0f : 1.0f, 0.0f);
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(vertex) = glm::vec3(1.0f, 0.0f, 0.0f);
							newVertices.get<Bitangent>(vertex) = glm::cross(newVertices.get<Normal>(vertex), newVertices.get<Tangent>(vertex));
						}
					}
				}

				// Y- face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				for (size_t vertex = currentVertex - 4; vertex < currentVertex; vertex++)
				{
					if (_shapeParams.generateNormals)
					{
						newVertices.get<Normal>(vertex) = glm::vec3(0.0f, _shapeParams.invertNormals ? 1.0f : -1.0f, 0.0f);
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(vertex) = glm::vec3(0.0f, 0.0f, 1.0f);
							newVertices.get<Bitangent>(vertex) = glm::cross(newVertices.get<Normal>(vertex), newVertices.get<Tangent>(vertex));
						}
					}
				}

				// X+ face
				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				for (size_t vertex = currentVertex - 4; vertex < currentVertex; vertex++)
				{
					if (_shapeParams.generateNormals)
					{
						newVertices.get<Normal>(vertex) = glm::vec3(_shapeParams.invertNormals ? -1.0f : 1.0f, 0.0f, 0.0f);
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(vertex) = glm::vec3(0.0f, 0.0f, 1.0f);
							newVertices.get<Bitangent>(vertex) = glm::cross(newVertices.get<Normal>(vertex), newVertices.get<Tangent>(vertex));
						}
					}
				}

				// X- face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				for (size_t vertex = currentVertex - 4; vertex < currentVertex; vertex++)
				{
					if (_shapeParams.generateNormals)
					{
						newVertices.get<Normal>(vertex) = glm::vec3(_shapeParams.invertNormals ? 1.0f : -1.0f, 0.0f, 0.0f);
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(vertex) = glm::vec3(0.0f, 0.0f, -1.0f);
							newVertices.get<Bitangent>(vertex) = glm::cross(newVertices.get<Normal>(vertex), newVertices.get<Tangent>(vertex));
						}
					}
				}

				// Z+ face
				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, 0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				for (size_t vertex = currentVertex - 4; vertex < currentVertex; vertex++)
				{
					if (_shapeParams.generateNormals)
					{
						newVertices.get<Normal>(vertex) = glm::vec3(0.0f, 0.0f, _shapeParams.invertNormals ? -1.0f : 1.0f);
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(vertex) = glm::vec3(-1.0f, 0.0f, 0.0f);
							newVertices.get<Bitangent>(vertex) = glm::cross(newVertices.get<Normal>(vertex), newVertices.get<Tangent>(vertex));
						}
					}
				}

				// Z- face
				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, -0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, -0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 0.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(0.5f, 0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(1.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				newVertices.get<Position>(currentVertex) = glm::vec3(-0.5f, 0.5f, -0.5f);
				newVertices.get<TexCoord0>(currentVertex) = glm::vec2(0.0f, 1.0f) * _shapeParams.uvScale;
				currentVertex++;

				for (size_t vertex = currentVertex - 4; vertex < currentVertex; vertex++)
				{
					if (_shapeParams.generateNormals)
					{
						newVertices.get<Normal>(vertex) = glm::vec3(0.0f, 0.0f, _shapeParams.invertNormals ? 1.0f : -1.0f);
						if (_shapeParams.generateTangents)
						{
							newVertices.get<Tangent>(vertex) = glm::vec3(1.0f, 0.0f, 0.0f);
							newVertices.get<Bitangent>(vertex) = glm::cross(newVertices.get<Normal>(vertex), newVertices.get<Tangent>(vertex));
						}
					}
				}
#pragma endregion

				vertexBufferCaches.push_back(std::make_unique<VertexBufferCache>(_shapeType, _shapeParams, newVertexBuffer));
				return newVertexBuffer;
			}
			else if (_shapeType == ShapeType::Sphere)
			{
				constexpr unsigned int minRes = 8;
				if (_shapeParams.resolution > 0 && _shapeParams.resolution < minRes)
					log::warning("Shape: Minimum sphere resolution is " + std::to_string(minRes) + "!");
				const unsigned int resolution = _shapeParams.resolution >= minRes ? _shapeParams.resolution : defaultCircleResolution;

				auto it = std::find_if(vertexBufferCaches.begin(), vertexBufferCaches.end(),
									   [&](const std::unique_ptr<VertexBufferCache>& _vbc)
									   {
										   return _vbc->type == _shapeType && _vbc->params == _shapeParams;
									   });
				if (it != vertexBufferCaches.end())
					return it->get()->buffer;


				std::shared_ptr<VertexBuffer> newVertexBuffer = std::make_shared<VertexBuffer>();
				VertexBuffer& newVertices = *newVertexBuffer.get();
				using namespace VertexAttribute;
				VertexAttributeFlagsType attributes = Position | TexCoord0;
				if (_shapeParams.generateNormals)
					enableBit(attributes, Normal);
				if (_shapeParams.generateTangents)
				{
					enableBit(attributes, Tangent);
					enableBit(attributes, Bitangent);
				}
				newVertices.setAttributes(attributes);

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
					const float phi = se::lerp(0.0f, TWO_PI<float>, i / res);
					for (float j = 0.0f; j <= res; j += 1.0f)
					{
						const float theta = se::lerp(0.0f, PI<float>, j / res);

						newVertices.get<Position>(currentVertex) = glm::vec3(radius * sinf(theta) * cosf(phi),
																			 radius * cosf(theta),
																			 radius * sinf(theta) * sinf(phi));
						newVertices.get<TexCoord0>(currentVertex) = glm::vec2(i / resolution, j / resolution) * _shapeParams.uvScale;
						if (_shapeParams.generateNormals)
						{
							newVertices.get<Normal>(currentVertex) = glm::normalize(newVertices.get<Position>(currentVertex)) * (_shapeParams.invertNormals ? -1.0f : 1.0f);
							if (_shapeParams.generateTangents)
							{
								if (theta == 0.0f || theta == PI<float>)
									newVertices.get<Tangent>(currentVertex) = glm::vec3(1.0f, 0.0f, 0.0f);
								else
									newVertices.get<Tangent>(currentVertex) = glm::vec3(-sinf(phi), 0, cosf(phi));
								newVertices.get<Bitangent>(currentVertex) = glm::cross(newVertices.get<Normal>(currentVertex), newVertices.get<Tangent>(currentVertex));
							}
						}
						currentVertex++;
					}
				}

				vertexBufferCaches.push_back(std::make_unique<VertexBufferCache>(_shapeType, _shapeParams, newVertexBuffer));
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
				case PrimitiveType::Triangles:
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
				case PrimitiveType::Lines:
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
				case PrimitiveType::Points:
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
