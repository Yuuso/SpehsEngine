
#include "Component.h"
#include "GameObject.h"


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

	void Component::update()
	{
		//Default update
	}
}