
#include "SpehsEngine/Game/GameObject.h"
#include "SpehsEngine/Game/Component.h"


namespace se
{
	GameObject::GameObject() : name("")
	{
		
	}
	GameObject::GameObject(const std::string& _name) : name(_name)
	{

	}
	GameObject::~GameObject()
	{
		for (unsigned i = 0; i < components.size(); i++)
		{
			delete components[i];
		}
	}


	void GameObject::update(const time::Time deltaTime)
	{
		for (unsigned i = 0; i < components.size(); i++)
		{
			components[i]->update(deltaTime);
		}
	}
}