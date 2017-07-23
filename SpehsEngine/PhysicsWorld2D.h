
#pragma once

#include "Vector.h"

#include <vector>
#include <memory>


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

		void enableGravity(const bool _value);

		//Setters
		void setGravity(const spehs::vec2& _gravity);

		//Getters
		spehs::vec2 getGravity() const{ return gravity; }

	private:
		//Enter mass as 0.0f for static bodies
		float j_lin(const float& _e, const spehs::vec2& _velocity, const spehs::vec2& _normal, const float& _mass1, const float& _mass2);
		float j_rot(const float& _e, const spehs::vec2& _velocity, const spehs::vec2& _normal, const float& _mass1, const float& _mass2, 
			const spehs::vec2& _rVecAP, const spehs::vec2& _rVecBP, const float& _MoIA, const float& _MoIB);

		bool useGravity;

		std::shared_ptr<CollisionPoint> collisionPoint;
		std::vector<RigidBody2D*> bodies;
		spehs::vec2 gravity;
	};
}

