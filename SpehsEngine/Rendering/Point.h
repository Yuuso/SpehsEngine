#pragma once

#include "SpehsEngine/Rendering/Primitive.h"

#include <string>

namespace se
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
		void setTexture(TextureData* _textureDataPtr);

	protected:
		//NOTE: Point always in 2D (No Z-Depth), if there is need for 3D points for some reason -> call setBlending(false)
		Point(BatchManager& _batchManager, const PlaneDepth &_planeDepth);
		~Point();
	};
}
