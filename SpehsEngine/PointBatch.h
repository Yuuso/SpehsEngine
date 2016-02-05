#pragma once

#include "PrimitiveBatch.h"


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
		void setTexture(unsigned int _textureIndex); //Set custom shaders AFTER calling setTexture
	};
}