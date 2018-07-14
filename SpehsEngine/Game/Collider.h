
#pragma once

#include "SpehsEngine/Game/Component.h"


namespace se
{
	namespace rendering
	{
		class Polygon;
	}
	class GameObject;
	class Collider : public Component
	{
		friend class RigidBody2D;
	public:
		Collider();
		Collider(GameObject& _owner);
		~Collider();

	protected:
		rendering::Polygon* collisionPolygon;
	};
}
