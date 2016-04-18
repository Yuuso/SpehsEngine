
#include "GameObject.h"
#include "Component.h"


namespace spehs
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


	void GameObject::update()
	{
		for (unsigned i = 0; i < components.size(); i++)
		{
			components[i]->update();
		}
	}
}