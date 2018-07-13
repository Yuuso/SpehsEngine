#include "stdafx.h"

#include "SpehsEngine/Core/Exceptions.h"
#include "SpehsEngine/Core/Shapes.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Rendering/Camera2D.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>



namespace se
{
	namespace rendering
	{
		Polygon::Polygon(BatchManager& _batchManager, const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height)
			: Polygon(_batchManager, _width, _height)
		{
			planeDepth = _planeDepth;
			if (_shapeID >= 3) //Regular Convex Polygons
			{
				vertexArray.resize(_shapeID);
				float firstPosition;
				//firstPosition adjusts initial the rotation for even numbered polygons
				//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
				if (_shapeID % 2)
					firstPosition = 0;
				else
					firstPosition = (TWO_PI / _shapeID) / 2.0f;
				vertexArray[0].position = se::vec2(cos(firstPosition), sin(firstPosition));
				float minX = vertexArray[0].position.x, minY = vertexArray[0].position.y, maxX = vertexArray[0].position.x, maxY = vertexArray[0].position.y;
				for (int i = 1; i < _shapeID; i++)
				{
					//Set position
					vertexArray[i].position = se::vec2(cos(firstPosition + i * (TWO_PI / _shapeID)), sin(firstPosition + i * (TWO_PI / _shapeID)));

					//Check min/max
					if (vertexArray[i].position.x > maxX)
						maxX = vertexArray[i].position.x;
					else if (vertexArray[i].position.x < minX)
						minX = vertexArray[i].position.x;
					if (vertexArray[i].position.y > maxY)
						maxY = vertexArray[i].position.y;
					else if (vertexArray[i].position.y < minY)
						minY = vertexArray[i].position.y;
				}
				float twidth = abs(maxX) + abs(minX);
				float theight = abs(maxY) + abs(minY);

				for (int i = 0; i < _shapeID; i++)
				{
					vertexArray[i].position.x /= twidth;
					vertexArray[i].position.y /= theight;
				}

				worldVertexArray = vertexArray;

				setUVCoords();
			}
			else //Polygons from Shapes
			{
				vertexArray = getShapeVertexArray(se::Shape(_shapeID));
				worldVertexArray = vertexArray;

				//Custom UV Coords for Button
				if (_shapeID == se::Shape::BUTTON)
				{
					for (size_t i = 0; i < worldVertexArray.size(); i++)
					{
						worldVertexArray[i].uv.u = (worldVertexArray[i].position.x);
						worldVertexArray[i].uv.v = (-worldVertexArray[i].position.y);
					}
				}
				else
				{
					setUVCoords();
				}
			}
		}

		Polygon::Polygon(BatchManager& _batchManager, const std::vector<se::Vertex>& _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height)
			: Polygon(_batchManager, _width, _height)
		{
			planeDepth = _planeDepth;
			if (_vertexData.size() < 3)
				exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

			vertexArray = _vertexData;
			worldVertexArray = vertexArray;

			setUVCoords();
		}

		Polygon::Polygon(BatchManager& _batchManager, const std::vector<se::Vertex>& _vertexData, const float _width, const float _height)
			: Polygon(_batchManager, _vertexData, 0, _width, _height)
		{
			blending = false;
		}

		Polygon::Polygon(BatchManager& _batchManager, const std::vector<se::vec2>& _positionData, const PlaneDepth _planeDepth, const float _width, const float _height)
			: Polygon(_batchManager, _width, _height)
		{
			planeDepth = _planeDepth;
			if (_positionData.size() < 3)
				exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

			vertexArray.resize(_positionData.size());
			for (size_t i = 0; i < _positionData.size(); i++)
			{
				vertexArray[i].position = _positionData[i];
			}
			worldVertexArray = vertexArray;

			setUVCoords();
		}

		Polygon::Polygon(BatchManager& _batchManager, const float _width, const float _height)
			: Primitive(_batchManager)
			, width(_width)
			, height(_height)
		{
			drawMode = DrawMode::TRIANGLE;
			blending = true;
		}

		Polygon::~Polygon()
		{

		}

		void Polygon::updateVertices()
		{
			if (needUpdate)
			{
				glm::vec4 vertex;
				scaledMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, 0.0f));
				scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, /*TODO1*/glm::vec3(rotationVector.x, rotationVector.y, rotationVector.z));
				for (size_t i = 0; i < worldVertexArray.size(); i++)
				{
					vertex = scaledRotatedMatrix * glm::vec4(vertexArray[i].position.x * width, vertexArray[i].position.y * height, 0.0f, 1.0f);
					worldVertexArray[i].position = se::vec2(vertex.x + position.x, vertex.y + position.y);
				}
				needUpdate = false;
			}
		}

		void Polygon::getScreenVertices(Camera2D* _camera, std::vector<se::vec2>& deposit)
		{
			if (cameraMatrixState)
			{
				for (size_t i = 0; i < worldVertexArray.size(); i++)
				{
					glm::vec2 vec(*_camera->projectionMatrix * glm::vec4(worldVertexArray[i].position.x, worldVertexArray[i].position.y, 0.0f, 1.0f));
					deposit.push_back(se::vec2(vec.x, vec.y));
				}
			}
			else
			{
				se::exceptions::unexpectedError("Camera state not enabled, screen vertices function is useless.");
			}
		}

		void Polygon::resize(const float _width, const float _height)
		{
			width = _width;
			height = _height;
			needUpdate = true;
		}

		void Polygon::setUVScale(const float _newScale)
		{
			for (size_t i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].uv.u *= _newScale;
				worldVertexArray[i].uv.v *= _newScale;
			}
		}

		void Polygon::setUVScale(const float _newScaleX, const float _newScaleY)
		{
			for (size_t i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].uv.u *= _newScaleX;
				worldVertexArray[i].uv.v *= _newScaleY;
			}
		}

		void Polygon::setUVScale(const se::vec2& _newScale)
		{
			for (size_t i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].uv.u *= _newScale.x;
				worldVertexArray[i].uv.v *= _newScale.y;
			}
		}

		void Polygon::setDrawMode(const DrawMode _newDrawMode)
		{
			drawMode = _newDrawMode;
		}

		TextureData* Polygon::setTexture(const std::string& _texturePath)
		{
			TextureData* value = batchManager.textureManager.getTextureData(_texturePath);
			textureDataID = value->textureDataID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultPolygon)
				shaderIndex = (unsigned int)ShaderName::DefaultTexture;
			return value;
		}

		TextureData* Polygon::setTexture(const size_t _hash)
		{
			TextureData* value = batchManager.textureManager.getTextureData(_hash);
			textureDataID = value->textureDataID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultPolygon)
				shaderIndex = (unsigned int)ShaderName::DefaultTexture;
			return value;
		}

		void Polygon::setTexture(const TextureData* _textureDataPtr)
		{
			textureDataID = _textureDataPtr->textureDataID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultPolygon)
				shaderIndex = (unsigned int)ShaderName::DefaultTexture;
		}

		void Polygon::setTextureID(const unsigned int _textureID)
		{
			textureDataID = _textureID;
			if (shaderIndex == (unsigned int)ShaderName::DefaultPolygon)
				shaderIndex = (unsigned int)ShaderName::DefaultTexture;
		}

		float Polygon::getArea()
		{
			if (worldVertexArray.size() <= 2u)
				return 0.0f;
			float area = 0.0f;
			size_t j = size_t(int(worldVertexArray.size()) - 1);
			for (size_t i = 0; i < worldVertexArray.size(); i++)
			{
				area += (worldVertexArray[j].position.x * worldVertexArray[i].position.y) - (worldVertexArray[i].position.x * worldVertexArray[j].position.y);
				j = i;
			}
			return abs(area / 2.0f);
		}

		float Polygon::getRadius()
		{
			//Seems like this needs to be updated every time
			float max = (se::vec2(position.x, position.y) - se::vec2(worldVertexArray[0].position.x, worldVertexArray[0].position.y)).getLength();
			for (size_t i = 1; i < worldVertexArray.size(); i++)
			{
				if ((se::vec2(position.x, position.y) - se::vec2(worldVertexArray[i].position.x, worldVertexArray[i].position.y)).getLength() > max)
				{
					max = (se::vec2(position.x, position.y) - se::vec2(worldVertexArray[i].position.x, worldVertexArray[i].position.y)).getLength();
				}
			}
			radius = max;
			return radius;
		}

		void Polygon::setUVCoords()
		{
			for (size_t i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].uv.u = (worldVertexArray[i].position.x + 0.5f);
				worldVertexArray[i].uv.v = (-worldVertexArray[i].position.y + 0.5f);
			}
		}
	}
}
