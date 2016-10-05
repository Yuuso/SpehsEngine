
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
	}

	void Sprite::setPolygon(float _x, float _y, int _numVertices)
	{
		sprite->destroy();
		sprite = spehs::Polygon::create(_numVertices, 0, _x, _y);
	}
}