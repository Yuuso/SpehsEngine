
#include "SpehsEngine/Game/Sprite.h"
#include "SpehsEngine/Game/GameObject.h"
#include "SpehsEngine/Game/Transform2D.h"
#include "SpehsEngine/Rendering/Polygon.h"
#include "SpehsEngine/Rendering/TextureManager.h"
#include "SpehsEngine/Rendering/BatchManager.h"
#include "SpehsEngine/Core/Exceptions.h"


#define DEFAULT_SIZE 50.0f

namespace spehs
{
	Sprite::Sprite(BatchManager& batchManager)
	{
		sprite = batchManager.createPolygon(4, 0, DEFAULT_SIZE, DEFAULT_SIZE);
	}
	Sprite::Sprite(BatchManager& batchManager, GameObject& _owner) : Component(_owner)
	{
		sprite = batchManager.createPolygon(4, 0, DEFAULT_SIZE, DEFAULT_SIZE);
	}
	Sprite::~Sprite()
	{
		sprite->destroy();
	}


	void Sprite::update(const time::Time deltaTime)
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
	void Sprite::setColor(const spehs::Color& _newColor)
	{
		sprite->setColor(_newColor);
	}
	void Sprite::setAlpha(const spehs::Color::Component& _a)
	{
		sprite->setAlpha(_a);
	}
	void Sprite::setDepth(const int _depth)
	{
		sprite->setPlaneDepth(_depth);
	}
	void Sprite::setRenderState(const bool _state)
	{
		sprite->setRenderState(_state);
	}
	void Sprite::updateVertices()
	{
		sprite->updateVertices();
	}
}