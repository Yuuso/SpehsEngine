
#pragma once

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
		type* cast = new type(this);

		//if (std::is_base_of<Component, type>::value == false)
		//{
		//	std::cout << "Given type in not a Component!" << std::endl;
		//	return;
		//}
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
		std::cout << "Component not found!" << std::endl;
		return nullptr;
	}
}
