
#include "Point.h"
#include "BatchManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Vertex.h"


namespace spehs
{
	Point::Point(const PlaneDepth &_planeDepth)
	{
		numVertices = 1;
		Vertex* pos = new Vertex[numVertices];
		pos[0].position.setPosition(0.0f, 0.0f);

		vertexArray = pos;
		worldVertexArray = new Vertex[numVertices];
		std::copy(pos, pos + numVertices, stdext::checked_array_iterator<Vertex*>(worldVertexArray, numVertices));

		planeDepth = _planeDepth;
		blending = true;
		drawMode = POINT;
	}
	Point::~Point()
	{

	}


	void Point::updateVertices()
	{
		for (unsigned int i = 0; i < numVertices; i++)
		{
			worldVertexArray[i].position.setPosition(vertexArray[i].position.x + position.x, vertexArray[i].position.y + position.y, vertexArray[i].position.z + position.z);
		}
	}

	TextureData* Point::setTexture(const std::string &_texturePath)
	{
		textureDataID = textureManager->preloadTexture(_texturePath);
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return textureManager->getTextureData(textureDataID);
	}

	TextureData* Point::setTexture(const size_t &_textureID)
	{
		textureDataID = _textureID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return textureManager->getTextureData(textureDataID);
	}
}
