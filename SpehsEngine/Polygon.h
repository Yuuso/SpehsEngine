#pragma once

#include "Primitive.h"
#include "Vertex.h"

#include <glm/vec2.hpp>

#include <string>


typedef unsigned int GLuint;

namespace spehs
{
	struct TextureData;
	class Camera2D;

	class Polygon : public Primitive
	{
		friend class BatchManager;
		friend class RigidBody2D;

	public:
		static Polygon* create(const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height);
		static Polygon* create(const std::vector<spehs::Vertex>& _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height);
		static Polygon* create(const std::vector<spehs::Vertex>& _vertexData, const float _width, const float _height);
		static Polygon* create(const std::vector<glm::vec2>& _cuspData, const PlaneDepth _planeDepth, const float _width, const float _height);

		Polygon* getPolygonPtr(){ return this; }

		void updateVertices();

		std::vector<glm::vec2>* getScreenVertices(spehs::Camera2D* _camera);

		void resize(const float &_width, const float &_height);
		//Setters
		void setUVScale(const float &_newScale);
		void setUVScale(const float &_newScaleX, const float &_newScaleY);
		void setUVScale(const glm::vec2 &_newScale);
		void setDrawMode(const DrawMode &_newDrawMode);
		TextureData* setTexture(const std::string &_texturePath);
		TextureData* setTexture(const size_t &_hash);
		void setTexture(const TextureData* _textureDataPtr);
		void setTextureID(const unsigned int &_textureID);

		//Getters
		float getArea();
		float getRadius();
		float getWidth() const { return width; }
		float getHeight() const { return height; }

	protected:
		Polygon(const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height);
		Polygon(const std::vector<spehs::Vertex>& _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height); //For 2DPolygons
		Polygon(const std::vector<spehs::Vertex>& _vertexData, const float _width, const float _height);
		Polygon(const std::vector<glm::vec2>& _cuspData, const PlaneDepth _planeDepth, const float _width, const float _height);
		~Polygon();

		void setUVCoords();

		float radius;
		float width, height;

	private:
		Polygon(const float _width, const float _height);
	};
}
