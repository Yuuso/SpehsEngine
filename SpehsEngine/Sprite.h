
#pragma once

#include "Component.h"
#include "Primitive.h"


namespace spehs
{
	class GameObject;
	class Polygon;
	class TextureData;

	class Sprite : public Component, public Colorable
	{
	public:
		Sprite();
		Sprite(GameObject& _owner);
		~Sprite() override;

		void update(const time::Time deltaTime) override;

		virtual void setTexture(TextureData* _textureDataPtr);
		virtual void setTextureID(const unsigned int _textureID);
		void setSize(const float _x, const float _y);
		void setColor(const spehs::Color &_newColor) override;
		void setAlpha(const spehs::Color::Component& _a) override;
		void setDepth(const int _depth);
		void setRenderState(const bool _state);
		void updateVertices();
		const Polygon* getSpritePolygon(){ return sprite; }

	protected:
		Polygon* sprite;
	};
}

