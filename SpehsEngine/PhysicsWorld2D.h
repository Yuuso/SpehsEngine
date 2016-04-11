
#pragma once
#include <vector>


namespace spehs
{
	class RigidBody2D;

	class PhysicsWorld2D
	{
	public:
		PhysicsWorld2D();
		~PhysicsWorld2D();
		friend RigidBody2D;

		void update();
		bool addRigidBody(RigidBody2D& _body);
		void removeRigidBody(RigidBody2D& _body);

	private:
		std::vector<RigidBody2D*> bodies;
	};
}

