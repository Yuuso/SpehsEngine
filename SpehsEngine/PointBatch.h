#pragma once

#include "PrimitiveBatch.h"

#include <string>


namespace SpehsEngine
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
		void setTexture(std::string _texturePath);
		void setTexture(size_t _textureID);
	};
}