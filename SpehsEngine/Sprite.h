
#pragma once

#include "Component.h"
#include "Primitive.h"


namespace spehs
{
	class GameObject;
	class Polygon;
	class TextureData;

	class Sprite : public Component
	{
	public:
		Sprite();
		Sprite(GameObject& _owner);
		~Sprite() override;

		void update() override;

		virtual void setTexture(TextureData* _textureDataPtr);
		virtual void setTextureID(const unsigned int _textureID);
		void setSize(const float _x, const float _y);
		void setColor(const glm::vec4 &_newColor);
		void setColor(const glm::vec3 &_newColor);
		void setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a = 255);
		void setColor(const CommonColor& _color);
		void setColorAlpha(const float _a);
		void setColorAlpha(const unsigned char _a);
		void setDepth(const int _depth);
		const Polygon* getSpritePolygon(){ return sprite; }

	protected:
		Polygon* sprite;
	};
}

