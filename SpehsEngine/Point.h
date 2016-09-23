#pragma once

#include "Primitive.h"

#include <string>

namespace spehs
{
	struct TextureData;

	class Point : public Primitive
	{
		friend class BatchManager;

	public:
		static Point* create(const PlaneDepth &_planeDepth);

		Point* getPointPtr(){ return this; }

		void updateVertices();

		//Scaling or rotating will not affect points

		TextureData* setTexture(const std::string &_texturePath);
		TextureData* setTexture(const size_t &_textureID);
		void setTexture(TextureData* _textureDataPtr);

	protected:
		//NOTE: Point always in 2D (No Z-Depth), if there is need for 3D points for some reason -> call setBlending(false)
		Point(const PlaneDepth &_planeDepth);
		~Point();
	};
}
