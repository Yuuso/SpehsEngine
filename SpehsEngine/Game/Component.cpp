
#include "SpehsEngine/Game/Component.h"
#include "SpehsEngine/Game/GameObject.h"


namespace spehs
{
	Component::Component()
	{
		ownerObject = nullptr;
	}
	Component::Component(GameObject& _owner)
	{
		ownerObject = &_owner;
	}
	Component::~Component()
	{

	}

	void Component::update(const time::Time deltaTime)
	{
		//Default update
	}
}