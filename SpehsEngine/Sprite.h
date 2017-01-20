
#pragma once

#include "Component.h"


namespace spehs
{
	class GameObject;
	class Polygon;

	class Sprite : public Component
	{
	public:
		Sprite();
		Sprite(GameObject& _owner);
		virtual ~Sprite();

		virtual void update();

		void setSize(const float _x, const float _y);
		void setTexture(TextureData* _textureDataPtr);
		void setTextureID(const unsigned int _textureID);
		void setDepth(const int _depth);

	protected:
		Polygon* sprite;
	};
}

