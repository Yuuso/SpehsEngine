#pragma once

#include "Primitive.h"

#include <string>

namespace SpehsEngine
{
	struct TextureData;

	class Point : public Primitive
	{
		friend class BatchManager;

	public:
		Point* getPointPtr(){ return this; }

		void updateVertices();

		//Scaling or rotating will not affect points

		TextureData* setTexture(const std::string &_texturePath);
		TextureData* setTexture(const size_t &_textureID);

	protected:
		//NOTE: Point always in 2D (No Z-Depth), if there is need for 3D points for some reason -> call setBlending(false)
		Point(const PlaneDepth &_planeDepth = 0);
		~Point();
	};
}
