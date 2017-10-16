
#include "Vertex.h"
#include "Polygon.h"
#include "TextureManager.h"
#include "Shapes.h"
#include "Exceptions.h"
#include "BatchManager.h"
#include "ShaderManager.h"
#include "Exceptions.h"
#include "Camera2D.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>



namespace spehs
{
	Polygon* Polygon::create(const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height)
	{
		return getActiveBatchManager()->createPolygon(_shapeID, _planeDepth, _width, _height);
	}
	Polygon* Polygon::create(const std::vector<spehs::Vertex>& _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height)
	{
		return getActiveBatchManager()->createPolygon(_vertexData, _planeDepth, _width, _height);
	}
	Polygon* Polygon::create(const std::vector<spehs::Vertex>& _vertexData, const float _width, const float _height)
	{
		return getActiveBatchManager()->createPolygon(_vertexData, _width, _height);
	}
	Polygon* Polygon::create(const std::vector<spehs::vec2>& _cuspData, const PlaneDepth _planeDepth, const float _width, const float _height)
	{
		return getActiveBatchManager()->createPolygon(_cuspData, _planeDepth, _width, _height);
	}
	Polygon::Polygon(const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height)
		: Polygon(_width, _height)
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
			vertexArray[0].position = spehs::vec2(cos(firstPosition), sin(firstPosition));
			float minX = vertexArray[0].position.x, minY = vertexArray[0].position.y, maxX = vertexArray[0].position.x, maxY = vertexArray[0].position.y;
			for (int i = 1; i < _shapeID; i++)
			{
				//Set position
				vertexArray[i].position = spehs::vec2(cos(firstPosition + i * (TWO_PI / _shapeID)), sin(firstPosition + i * (TWO_PI / _shapeID)));

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
			vertexArray = getShapeVertexArray(spehs::Shape(_shapeID));
			worldVertexArray = vertexArray;

			//Custom UV Coords for Button
			if (_shapeID == spehs::Shape::BUTTON)
			{
				for (unsigned int i = 0; i < worldVertexArray.size(); i++)
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
	Polygon::Polygon(const std::vector<spehs::Vertex>& _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height)
		: Polygon(_width, _height)
	{
		planeDepth = _planeDepth;
		if (_vertexData.size() < 3)
			exceptions::fatalError("Can't create a polygon with less than 3 vertices!");

		vertexArray = _vertexData;
		worldVertexArray = vertexArray;

		setUVCoords();
	}
	Polygon::Polygon(const std::vector<spehs::Vertex>& _vertexData, const float _width, const float _height)
		: Polygon(_vertexData, 0, _width, _height)
	{
		blending = false;
	}
	Polygon::Polygon(const std::vector<spehs::vec2>& _positionData, const PlaneDepth _planeDepth, const float _width, const float _height)
		: Polygon(_width, _height)
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
	Polygon::Polygon(const float _width, const float _height)
		: width(_width)
		, height(_height)
		, radius(0.0f)
	{
		drawMode = TRIANGLE;
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
			for (unsigned int i = 0; i < worldVertexArray.size(); i++)
			{
				vertex = scaledRotatedMatrix * glm::vec4(vertexArray[i].position.x * width, vertexArray[i].position.y * height, 0.0f, 1.0f);
				worldVertexArray[i].position = spehs::vec2(vertex.x + position.x, vertex.y + position.y);
			}
			needUpdate = false;
		}
	}


	void Polygon::getScreenVertices(spehs::Camera2D* _camera, std::vector<spehs::vec2>& deposit)
	{
		if (cameraMatrixState)
		{
			for (unsigned i = 0; i < worldVertexArray.size(); i++)
			{
				glm::vec2 vec(*_camera->projectionMatrix * glm::vec4(worldVertexArray[i].position.x, worldVertexArray[i].position.y, 0.0f, 1.0f));
				deposit.push_back(spehs::vec2(vec.x, vec.y));
			}
		}
		else
		{
			spehs::exceptions::unexpectedError("Camera state not enabled, screen vertices function is useless.");
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

	void Polygon::setUVScale(const spehs::vec2 &_newScale)
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

	TextureData* Polygon::setTexture(const size_t &_hash)
	{
		TextureData* value = textureManager->getTextureData(_hash);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return value;
	}

	void Polygon::setTexture(const TextureData* _textureDataPtr)
	{
		textureDataID = _textureDataPtr->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
	}

	void Polygon::setTextureID(const unsigned int &_textureID)
	{
		textureDataID = _textureID;
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
		//Seems like this needs to be updated every time
		float max = (spehs::vec2(position.x, position.y) - spehs::vec2(worldVertexArray[0].position.x, worldVertexArray[0].position.y)).getLength();
		for (unsigned int i = 1; i < worldVertexArray.size(); i++)
		{
			if ((spehs::vec2(position.x, position.y) - spehs::vec2(worldVertexArray[i].position.x, worldVertexArray[i].position.y)).getLength() > max)
			{
				max = (spehs::vec2(position.x, position.y) - spehs::vec2(worldVertexArray[i].position.x, worldVertexArray[i].position.y)).getLength();
			}
		}
		radius = max;
		return radius;
	}


	//Private:
	void Polygon::setUVCoords()
	{
		for (unsigned int i = 0; i < worldVertexArray.size(); i++)
		{
			worldVertexArray[i].uv.u = (worldVertexArray[i].position.x + 0.5f) ;
			worldVertexArray[i].uv.v = (-worldVertexArray[i].position.y + 0.5f) ;
		}
	}
}
