#pragma once

#include "Primitive.h"
#include "Vertex.h"

#include <glm/vec2.hpp>

#include <string>


typedef unsigned int GLuint;

namespace spehs
{
	struct TextureData;

	class Polygon : public Primitive
	{
		friend class BatchManager;

	public:
		Polygon* getPolygonPtr(){ return this; }

		void updateVertices();

		void resize(const float &_width, const float &_height);
		//Setters
		void setUVScale(const float &_newScale);
		void setUVScale(const float &_newScaleX, const float &_newScaleY);
		void setUVScale(const glm::vec2 &_newScale);
		void setDrawMode(const DrawMode &_newDrawMode);
		TextureData* setTexture(const std::string &_texturePath);
		TextureData* setTexture(const size_t &_textureID);
		void setTexture(TextureData* _textureDataPtr);

		//Getters
		float getArea();
		float getRadius();

	protected:
		Polygon(const int &_shapeID, const PlaneDepth &_planeDepth, const float &_width, const float &_height);
		Polygon(std::vector<spehs::Vertex> _vertexData, const PlaneDepth &_planeDepth, const float &_width, const float &_height); //For 2DPolygons
		Polygon(std::vector<spehs::Vertex> _vertexData, const float &_width, const float &_height);
		Polygon(std::vector<spehs::Position> _positionData, const PlaneDepth &_planeDepth, const float &_width, const float &_height);
		~Polygon();

		void setUVCoords();

		float radius;
		float width, height;

	private:
		Polygon(const float &_width, const float &_height);
	};
}
