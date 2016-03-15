
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
	Polygon::Polygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		if (_shapeID >= 3) //Regular Convex Polygons
		{
			numVertices = _shapeID;
			Vertex* pos = new Vertex[numVertices];
			float firstPosition;
			//firstPosition adjusts initial the rotation for even numbered polygons
			//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
			if (numVertices % 2)
				firstPosition = HALF_PI;
			else
				firstPosition = HALF_PI + (TWO_PI / numVertices) / 2;
			pos[0].setPosition(cos(firstPosition), sin(firstPosition));
			float minX = pos[0].position.x, minY = pos[0].position.y, maxX = pos[0].position.x, maxY = pos[0].position.y;
			for (int i = 1; i < numVertices; i++)
			{
				//Set position
				pos[i].setPosition(cos(firstPosition - i * (TWO_PI / numVertices)), sin(firstPosition - i * (TWO_PI / numVertices)));

				//Check min/max
				if (pos[i].position.x > maxX)
					maxX = pos[i].position.x;
				else if (pos[i].position.x < minX)
					minX = pos[i].position.x;
				if (pos[i].position.y > maxY)
					maxY = pos[i].position.y;
				else if (pos[i].position.y < minY)
					minY = pos[i].position.y;
			}
			float width = maxX - minX;//Note: Max x IS positive, min IS negative
			float height = maxY - minY;//Note: Max x IS positive, min IS negative
			float wFactor = 1.0f / width;
			float hFactor = 1.0f / height;

			for (int i = 0; i < numVertices; i++)
			{
				pos[i].position.x *= wFactor;
				pos[i].position.y *= hFactor;
			}

			vertexArray = pos;
			worldVertexArray = new Vertex[numVertices];
			std::copy(pos, pos + numVertices, stdext::checked_array_iterator<Vertex*>(worldVertexArray, numVertices));
		}
		else //Polygons from Shapes
		{
			numVertices = getNumVertices(spehs::Shape(_shapeID));
			if (numVertices < 3)
				exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

			Vertex* shapeVertexArray = getShapeVertexArray(spehs::Shape(_shapeID));
			if (shapeVertexArray == nullptr)
				exceptions::fatalError(__FUNCTION__" Vertex Array is a nullptr!");

			//Copy the shape vertex array into polygon's own arrays
			vertexArray = shapeVertexArray;
			worldVertexArray = new Vertex[numVertices];
			std::copy(shapeVertexArray, shapeVertexArray + numVertices, stdext::checked_array_iterator<Vertex*>(worldVertexArray, numVertices));
		}

		drawMode = POLYGON;
		setUVCoords();
		width = _width;
		height = _height;
		radius = 0.0f;
		planeDepth = _planeDepth;
		blending = true;
	}
	Polygon::Polygon(Vertex* _vertexData, const int &_numVertices, const PlaneDepth &_planeDepth, const float &_width, const float &_height)
	{
		planeDepth = _planeDepth;
		numVertices = _numVertices;
		if (numVertices < 3)
			exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

		if (_vertexData == nullptr)
			exceptions::fatalError("Vertex Array is a nullptr!");

		//Copy the shape vertex array into polygon's own arrays
		vertexArray = new Vertex[numVertices];
		std::copy(_vertexData, _vertexData + numVertices, stdext::checked_array_iterator<Vertex*>(vertexArray, numVertices));
		worldVertexArray = new Vertex[numVertices];
		std::copy(_vertexData, _vertexData + numVertices, stdext::checked_array_iterator<Vertex*>(worldVertexArray, numVertices));

		drawMode = POLYGON;
		setUVCoords();
		width = _width;
		height = _height;
		radius = 0.0f;
		blending = true;
	}
	Polygon::Polygon(Vertex* _vertexData, const int &_numVertices, const float &_width, const float &_height) : Polygon(_vertexData, _numVertices, 0, _width, _height)
	{
		blending = false;
	}
	Polygon::~Polygon()
	{

	}


	void Polygon::updateVertices()
	{
		static glm::vec4 vertex;
		scaledMatrix = glm::scale(glm::mat4(), glm::vec3(scaleX, scaleY, 0.0f));
		scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, rotationVector);
		for (unsigned int i = 0; i < numVertices; i++)
		{
			vertex = glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, vertexArray[i].position.z, 1.0f);
			vertex.x *= width;
			vertex.y *= height;
			vertex = scaledRotatedMatrix * vertex;
			worldVertexArray[i].setPosition(vertex.x + position.x, vertex.y + position.y, vertex.z + position.z);
		}
	}


	void Polygon::resize(const float &_width, const float &_height)
	{
		width = _width;
		height = _height;
	}

	void Polygon::setUVScale(const float &_newScale)
	{
		for (unsigned int i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].uv.u *= _newScale;
			worldVertexArray[i].uv.v *= _newScale;
		}
	}

	void Polygon::setUVScale(const float &_newScaleX, const float &_newScaleY)
	{
		for (unsigned int i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].uv.u *= _newScaleX;
			worldVertexArray[i].uv.v *= _newScaleY;
		}
	}

	void Polygon::setUVScale(const glm::vec2 &_newScale)
	{
		for (unsigned int i = 0; i < numVertices; i++)
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
		textureDataID = textureManager->getTextureData(_texturePath)->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return textureManager->getTextureData(textureDataID);
	}

	TextureData* Polygon::setTexture(const size_t &_textureID)
	{
		textureDataID = textureManager->getTextureData(_textureID)->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return textureManager->getTextureData(textureDataID);
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
		int j = numVertices - 1;
		for (unsigned int i = 0; i < numVertices; i++)
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
			for (unsigned int i = 1; i < numVertices; i++)
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
		for (unsigned int i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].uv.u = (worldVertexArray[i].position.x + 1.0f) / 2.0f;
			worldVertexArray[i].uv.v = (worldVertexArray[i].position.y + 1.0f) / 2.0f;
		}
	}
}
