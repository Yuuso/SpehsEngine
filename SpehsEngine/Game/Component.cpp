#include "stdafx.h"

#include "SpehsEngine/Game/Component.h"
#include "SpehsEngine/Game/GameObject.h"


namespace se
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