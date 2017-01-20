
#include "Sprite.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Polygon.h"
#include "Exceptions.h"


namespace spehs
{
	Sprite::Sprite()
	{
		sprite = spehs::Polygon::create(4, 0, 1.0f, 1.0f);
	}
	Sprite::Sprite(GameObject& _owner) : Component(_owner)
	{
		sprite = spehs::Polygon::create(4, 0, 1.0f, 1.0f);
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
	void Sprite::setDepth(const int _depth)
	{
		sprite->setPlaneDepth(_depth);
	}
}