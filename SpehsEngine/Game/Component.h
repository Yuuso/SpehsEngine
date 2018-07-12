
#pragma once
#include "SpehsEngine/Core/Time.h"

namespace se
{
	class GameObject;

	class Component
	{
	public:
		Component();
		Component(GameObject& _owner);
		virtual ~Component();

		virtual void update(const time::Time deltaTime);

	protected:
		GameObject* ownerObject;
	};
}

