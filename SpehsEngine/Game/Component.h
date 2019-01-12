
#pragma once
#include "SpehsEngine/Core/SE_Time.h"

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

