#pragma once

#include "SpehsEngine/Core/SE_Time.h"

#include <string>
#include <vector>

namespace se
{
	class Component;

	class GameObject
	{
	public:
		GameObject();
		GameObject(const std::string& _name);
		~GameObject();

		void update(const time::Time deltaTime);

		template <typename type> type* addComponent();
		template <typename type> type* getComponent();

		//Setters
		void setName(const std::string& _name){ name = _name; }

	private:
		std::string name;
		std::vector<Component*> components;
	};


	//Templates:

	template <typename type>
	type* GameObject::addComponent()
	{
		type* cast = new type(*this);

		if (!dynamic_cast<Component*>(cast))
		{
			log::error("Given type is not a Component.");
		}
		
		components.push_back(cast);
		return cast;
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
