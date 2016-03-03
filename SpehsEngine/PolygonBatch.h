#pragma once

#include "PrimitiveBatch.h"
#include "TextureManager.h"

#include <string>


namespace spehs
{
	class PolygonBatch : public PrimitiveBatch
	{
	public:
		PolygonBatch(int _shapeID, float _width = 1.0f, float _height = 1.0f);
		PolygonBatch(Vertex* _vertexData, int _numVertices, float _width = 1.0f, float _height = 1.0f);
		~PolygonBatch();

		void addPolygon(float _xDistanceFromOrigin, float _yDistanceFromOrigin, PolygonBatch* _newPolygon);
		void draw();
		void resize(float _width, float _height);
		void setPosition(float _x, float _y);
		void setScale(float _newScale);
		void setDrawMode(DrawMode _drawMode);
		TextureData* setTexture(std::string _texturePath);
		TextureData* setTexture(size_t _textureID);
		void setTexture(TextureData* _textureDataPtr);
		void setUVScale(float _newScale);

		void updateCollisions();
		float getArea();
		float getRadius(); //Max radius of the initial polygon

		Vertex* worldVertexArray = nullptr; //worldVertexArray is only updated for the initial polygon of the polygonBatch
		bool hasCircleHitbox = false;
		GLuint* numVertices = nullptr;
		GLuint totalNumVertices = 0;
	protected:
		void setPolygonIndices(DrawMode _drawMode);
		void updateVBO();

		int numPolygons = 0;
		float radius = 0.0f;
		DrawMode drawMode = UNDEFINED;
		Vertex worldPosition = Vertex();
		Vertex* originalVertexArray = nullptr; //This array should never be changed after polygon has been created.
	};
}