
#include "SpehsEngine/Rendering/Point.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/ShaderManager.h"
#include "SpehsEngine/Core/Vertex.h"


namespace spehs
{
	Point::Point(BatchManager& _batchManager, const PlaneDepth &_planeDepth)
		: Primitive(_batchManager)
	{
		vertexArray.push_back(Vertex());
		vertexArray.back().position = spehs::vec2(0.0f, 0.0f);

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
				worldVertexArray[i].position = spehs::vec2(vertexArray[i].position.x + position.x, vertexArray[i].position.y + position.y);
			}
			needUpdate = false;
		}
	}

	TextureData* Point::setTexture(const std::string &_texturePath)
	{
		TextureData* value = batchManager.textureManager.getTextureData(_texturePath);
		textureDataID = value->textureDataID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		return value;
	}

	TextureData* Point::setTexture(const size_t &_textureID)
	{
		TextureData* value = batchManager.textureManager.getTextureData(_textureID);
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
