#pragma once

#include "SpehsEngine/Rendering/Primitive.h"
#include "SpehsEngine/Core/Vertex.h"

#include <string>


typedef unsigned int GLuint;

namespace se
{
	struct TextureData;
	class Camera2D;

	class Polygon : public Primitive
	{
		friend class BatchManager;
		friend class RigidBody2D;

	public:

		Polygon* getPolygonPtr(){ return this; }

		void updateVertices();

		void getScreenVertices(se::Camera2D* _camera, std::vector<se::vec2>& deposit);

		void resize(const float _width, const float _height);
		//Setters
		void setUVScale(const float _newScale);
		void setUVScale(const float _newScaleX, const float _newScaleY);
		void setUVScale(const se::vec2& _newScale);
		void setDrawMode(const DrawMode _newDrawMode);
		TextureData* setTexture(const std::string& _texturePath);
		TextureData* setTexture(const size_t _hash);
		void setTexture(const TextureData* _textureDataPtr);
		void setTextureID(const unsigned int _textureID);

		//Getters
		float getArea();
		float getRadius();
		float getWidth() const { return width; }
		float getHeight() const { return height; }

	protected:
		Polygon(BatchManager& _batchManager, const int _shapeID, const PlaneDepth _planeDepth, const float _width, const float _height);
		Polygon(BatchManager& _batchManager, const std::vector<se::Vertex>& _vertexData, const PlaneDepth _planeDepth, const float _width, const float _height); //For 2DPolygons
		Polygon(BatchManager& _batchManager, const std::vector<se::Vertex>& _vertexData, const float _width, const float _height);
		Polygon(BatchManager& _batchManager, const std::vector<se::vec2>& _cuspData, const PlaneDepth _planeDepth, const float _width, const float _height);
		Polygon(BatchManager& _batchManager, const float _width, const float _height);
		~Polygon();

		void setUVCoords();

		float width;
		float height;
		float radius = 0.0f;
	};
}
