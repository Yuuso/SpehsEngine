
#include "Vertex.h"
#include "Polygon.h"
#include "TextureManager.h"
#include "Shapes.h"
#include "Console.h"
#include "BatchManager.h"
#include "ShaderManager.h"
#include "Exceptions.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>


#define HALF_PI 1.57079632679f
#define TWO_PI 6.28318530718f

#define GLM_FORCE_RADIANS


namespace spehs
{
	Polygon::Polygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height) : Polygon(_width, _height)
	{
		planeDepth = _planeDepth;
		if (_shapeID >= 3) //Regular Convex Polygons
		{
			vertexArray.resize(_shapeID);
			float firstPosition;
			//firstPosition adjusts initial the rotation for even numbered polygons
			//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
			if (_shapeID % 2)
				firstPosition = HALF_PI;
			else
				firstPosition = HALF_PI + (TWO_PI / _shapeID) / 2;
			vertexArray[0].position.setPosition(cos(firstPosition), sin(firstPosition));
			float minX = vertexArray[0].position.x, minY = vertexArray[0].position.y, maxX = vertexArray[0].position.x, maxY = vertexArray[0].position.y;
			for (int i = 1; i < _shapeID; i++)
			{
				//Set position
				vertexArray[i].position.setPosition(cos(firstPosition + i * (TWO_PI / _shapeID)), sin(firstPosition + i * (TWO_PI / _shapeID)));

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
			float width = maxX - minX;//Note: Max x IS positive, min IS negative
			float height = maxY - minY;//Note: Max x IS positive, min IS negative
			float wFactor = 1.0f / width;
			float hFactor = 1.0f / height;

			for (int i = 0; i < _shapeID; i++)
			{
				vertexArray[i].position.x *= wFactor;
				vertexArray[i].position.y *= hFactor;
			}

			worldVertexArray = vertexArray;
		}
		else //Polygons from Shapes
		{
			vertexArray = getShapeVertexArray(spehs::Shape(_shapeID));
			worldVertexArray = vertexArray;
		}

		setUVCoords();
	}
	Polygon::Polygon(std::vector<spehs::Vertex> _vertexData, const PlaneDepth &_planeDepth, const float &_width, const float &_height) : Polygon(_width, _height)
	{
		planeDepth = _planeDepth;
		if (_vertexData.size() < 3)
			exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

		vertexArray = _vertexData;
		worldVertexArray = vertexArray;

		setUVCoords();
	}
	Polygon::Polygon(std::vector<spehs::Vertex> _vertexData, const float &_width, const float &_height) : Polygon(_vertexData, 0, _width, _height)
	{
		blending = false;
	}
	Polygon::Polygon(std::vector<spehs::Position> _positionData, const PlaneDepth &_planeDepth, const float &_width, const float &_height) : Polygon(_width, _height)
	{
		planeDepth = _planeDepth;
		if (_positionData.size() < 3)
			exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

		vertexArray.resize(_positionData.size());
		for (unsigned i = 0; i < _positionData.size(); i++)
		{
			vertexArray[i].position = _positionData[i];
		}
		worldVertexArray = vertexArray;

		setUVCoords();
	}
	Polygon::Polygon(const float &_width, const float &_height)
	{
		drawMode = TRIANGLE;
		width = _width;
		height = _height;
		radius = 0.0f;
		blending = true;
	}
	Polygon::~Polygon()
	{

	}


	void Polygon::updateVertices()
	{
		if (needUpdate)
		{
			static glm::vec4 vertex;
			scaledMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, 0.0f));
			scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, rotationVector);
			for (unsigned int i = 0; i < worldVertexArray.size(); i++)
			{
				vertex = scaledRotatedMatrix * glm::vec4(vertexArray[i].position.x * width, vertexArray[i].position.y * height, vertexArray[i].position.z, 1.0f);
				worldVertexArray[i].position.setPosition(vertex.x + position.x, vertex.y + position.y, vertex.z + position.z);
			}
			needUpdate = false;
		}
	}


	void Polygon::resize(const float &_width, const float &_height)
	{
		width = _width;
		height = _height;
		needUpdate = true;
	}

	void Polygon::setUVScale(const float &_newScale)
	{
		for (unsigned int i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].uv.u *= _newScale;
			worldVertexArray[i].uv.v *= _newScale;
		}
	}

	void Polygon::setUVScale(const float &_newScaleX, const float &_newScaleY)
	{
		for (unsigned int i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].uv.u *= _newScaleX;
			worldVertexArray[i].uv.v *= _newScaleY;
		}
	}

	void Polygon::setUVScale(const glm::vec2 &_newScale)
	{
		for (unsigned int i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].uv.u *= _newScale.x;
			worldVertexArray[i].uv.v *= _newScale.y;
		}
	}

	void Polygon::setDrawMode(const DrawMode &_newDrawMode)
	{
		drawMode = _newDrawMode;
	}

	TextureData* Polygon::setTexture(const std::string &_texturePath)
	{
		TextureData* value = textureManager->getTextureData(_texturePath);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return value;
	}

	TextureData* Polygon::setTexture(const size_t &_textureID)
	{
		TextureData* value = textureManager->getTextureData(_textureID);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return value;
	}

	void Polygon::setTexture(TextureData* _textureDataPtr)
	{
		textureDataID = _textureDataPtr->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
	}

	float Polygon::getArea()
	{
		float area = 0.0f;
		int j = worldVertexArray.size() - 1;
		for (unsigned int i = 0; i < worldVertexArray.size(); i++)
		{
			area += (worldVertexArray[j].position.x * worldVertexArray[i].position.y) - (worldVertexArray[i].position.x * worldVertexArray[j].position.y);
			j = i;
		}
		return abs(area / 2.0f);
	}

	float Polygon::getRadius()
	{
		if (radius == 0.0f) //If it has not been calculated already
		{
			float max = glm::distance(glm::vec2(0.0f, 0.0f), glm::vec2(vertexArray[0].position.x, vertexArray[0].position.y));
			for (unsigned int i = 1; i < worldVertexArray.size(); i++)
			{
				if (glm::distance(glm::vec2(0.0f, 0.0f), glm::vec2(vertexArray[i].position.x, vertexArray[i].position.y)) > max)
				{
					max = glm::distance(glm::vec2(0.0f, 0.0f), glm::vec2(vertexArray[i].position.x, vertexArray[i].position.y));
				}
			}
			radius = max;
		}
		return radius;
	}


	//Private:
	void Polygon::setUVCoords()
	{
		for (unsigned int i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].uv.u = (worldVertexArray[i].position.x + 1.0f) / 2.0f;
			worldVertexArray[i].uv.v = (worldVertexArray[i].position.y + 1.0f) / 2.0f;
		}
	}
}
