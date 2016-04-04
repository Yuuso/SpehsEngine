
#pragma once


namespace spehs
{
	class GameObject;

	class Component
	{
	public:
		Component();
		Component(GameObject* _owner);
		virtual ~Component();

		virtual void update();

	protected:
		GameObject* ownerObject;
	};
}

