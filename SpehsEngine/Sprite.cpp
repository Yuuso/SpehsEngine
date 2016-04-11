
#include "Sprite.h"
#include "GameObject.h"
#include "BatchManager.h"


namespace spehs
{
	Sprite::Sprite()
	{
		sprite = getActiveBatchManager()->createPolygon(4);
	}
	Sprite::Sprite(GameObject& _owner) : Component(_owner) 
	{
		sprite = getActiveBatchManager()->createPolygon(4);
	}
	Sprite::~Sprite()
	{
		delete sprite;
	}
}