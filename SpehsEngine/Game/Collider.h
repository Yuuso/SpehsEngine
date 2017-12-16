
#pragma once

#include "SpehsEngine/Game/Component.h"


namespace spehs
{
	class GameObject;
	class Polygon;
	class Collider : public Component
	{
		friend class RigidBody2D;
	public:
		Collider();
		Collider(GameObject& _owner);
		~Collider();

	protected:
		Polygon* collisionPolygon;
	};
}
