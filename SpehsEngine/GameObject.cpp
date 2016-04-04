
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
		for (auto &it : components)
			delete it;
	}


	void GameObject::update()
	{
		for (auto &it : components)
			it->update();
	}
}