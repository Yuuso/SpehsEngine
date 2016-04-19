
#pragma once

#include <glm/vec2.hpp>
#include <vector>


namespace spehs
{
	class RigidBody2D;
	struct CollisionPoint;

	class PhysicsWorld2D
	{
		friend RigidBody2D;

	public:
		PhysicsWorld2D();
		~PhysicsWorld2D();

		bool addRigidBody(RigidBody2D& _body);
		void removeRigidBody(RigidBody2D& _body);

		void update();

		//Setters
		void setGravity(const glm::vec2& _gravity);

		//Getters
		glm::vec2 getGravity() const{ return gravity; }

	private:
		CollisionPoint* collisionPoint;
		std::vector<RigidBody2D*> bodies;
		glm::vec2 gravity;
	};
}

