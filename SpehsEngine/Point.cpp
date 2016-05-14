
#include "Point.h"
#include "BatchManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Vertex.h"


namespace spehs
{
	Point::Point(const PlaneDepth &_planeDepth)
	{
		vertexArray.push_back(Vertex());
		vertexArray.back().position.setPosition(0.0f, 0.0f);

		worldVertexArray = vertexArray;

		planeDepth = _planeDepth;
		blending = true;
		drawMode = POINT;
	}
	Point::~Point()
	{

	}


	void Point::updateVertices()
	{
		if (needUpdate)
		{
			for (unsigned int i = 0; i < worldVertexArray.size(); i++)
			{
				worldVertexArray[i].position.setPosition(vertexArray[i].position.x + position.x, vertexArray[i].position.y + position.y, vertexArray[i].position.z + position.z);
			}
			needUpdate = false;
		}
	}

	TextureData* Point::setTexture(const std::string &_texturePath)
	{
		TextureData* value = textureManager->getTextureData(_texturePath);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return value;
	}

	TextureData* Point::setTexture(const size_t &_textureID)
	{
		TextureData* value = textureManager->getTextureData(_textureID);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return value;
	}

	void Point::setTexture(TextureData* _textureDataPtr)
	{
		textureDataID = _textureDataPtr->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
	}
}
