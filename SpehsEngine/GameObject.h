
#pragma once

#include "Exceptions.h"

#include <string>
#include <vector>

namespace spehs
{
	class Component;

	class GameObject
	{
	public:
		GameObject();
		GameObject(const std::string& _name);
		~GameObject();

		void update();

		template <typename type> void addComponent();
		template <typename type> type* getComponent();

		//Setters
		void setName(const std::string& _name){ name = _name; }

	private:
		std::string name;
		std::vector<Component*> components;
	};


	//Templates:

	template <typename type>
	void GameObject::addComponent()
	{
		type* cast = new type(*this);

		if (!dynamic_cast<Component*>(cast))
		{
			exceptions::fatalError("Given type is not a Component.");
		}
		
		components.push_back(cast);
	}

	template <typename type>
	type* GameObject::getComponent()
	{
		type* temp;
		for (unsigned int i = 0; i < components.size(); i++)
		{
			temp = dynamic_cast<type*>(components[i]);
			if (temp != nullptr)
			{
				return dynamic_cast<type*>(components[i]);
			}
		}
		return nullptr;
	}
}
