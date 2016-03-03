#pragma once

#include "PrimitiveBatch.h"
#include "TextureManager.h"

#include <string>


namespace spehs
{
	class PointBatch : public PrimitiveBatch
	{
	public:
		PointBatch();
		~PointBatch();

		void draw();
		void setPosition(float _x, float _y);
		void addPoint(float _relativePosX, float _relativePosY);
		void setScale(float _newScale);
		TextureData* setTexture(std::string _texturePath);
		TextureData* setTexture(size_t _textureID);
	};
}