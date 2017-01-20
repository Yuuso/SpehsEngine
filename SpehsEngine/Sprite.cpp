
#include "Sprite.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Polygon.h"
#include "TextureManager.h"
#include "Exceptions.h"


#define DEFAULT_SIZE 50.0f

namespace spehs
{
	Sprite::Sprite()
	{
		sprite = spehs::Polygon::create(4, 0, DEFAULT_SIZE, DEFAULT_SIZE);
	}
	Sprite::Sprite(GameObject& _owner) : Component(_owner)
	{
		sprite = spehs::Polygon::create(4, 0, DEFAULT_SIZE, DEFAULT_SIZE);
	}
	Sprite::~Sprite()
	{
		sprite->destroy();
	}


	void Sprite::update()
	{
		Transform2D* transform = ownerObject->getComponent<Transform2D>();
		if (!transform)
			exceptions::fatalError("Object doesn't have transform component!");

		sprite->setPosition(transform->getPosition());
		sprite->setRotation(transform->getRotation());
		sprite->setScale(transform->getScale());
	}

	void Sprite::setSize(const float _x, const float _y)
	{
		sprite->resize(_x, _y);
	}
	void Sprite::setTexture(TextureData* _textureDataPtr)
	{
		sprite->setTexture(_textureDataPtr);
	}
	void Sprite::setTextureID(const unsigned int _textureID)
	{
		sprite->setTexture(_textureID);
	}
	void Sprite::setColor(const glm::vec4 &_newColor)
	{
		sprite->setColor(_newColor);
	}
	void Sprite::setColor(const glm::vec3 &_newColor)
	{
		sprite->setColor(_newColor);
	}
	void Sprite::setColor(const unsigned char _r, const unsigned char _g, const unsigned char _b, const unsigned char _a)
	{
		sprite->setColor(_r, _g, _b, _a);
	}
	void Sprite::setColor(const CommonColor& _color)
	{
		sprite->setColor(_color);
	}
	void Sprite::setColorAlpha(const float _a)
	{
		sprite->setColorAlpha(_a);
	}
	void Sprite::setColorAlpha(const unsigned char _a)
	{
		sprite->setColorAlpha(_a);
	}
	void Sprite::setDepth(const int _depth)
	{
		sprite->setPlaneDepth(_depth);
	}
}