#pragma once

#include "Primitive.h"

#include <string>

namespace SpehsEngine
{
	struct TextureData;

	class Point : public Primitive
	{
	public:
		Point(const PlaneDepth &_planeDepth = 0);

		Point* getPointPtr(){ return this; }

		void updateVertices();

		//Scaling or rotating will not affect points

		TextureData* setTexture(const std::string &_texturePath);
		TextureData* setTexture(const size_t &_textureID);

	protected:
		~Point();
	};
}
