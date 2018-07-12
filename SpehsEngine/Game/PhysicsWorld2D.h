
#pragma once

#include "SpehsEngine/Core/Vector.h"
#include "SpehsEngine/Physics/SATCollision.h"

#include <vector>
#include <memory>


namespace se
{
	class RigidBody2D;

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
		void setGravity(const se::vec2& _gravity);

		//Getters
		se::vec2 getGravity() const{ return gravity; }

	private:
		//Enter mass as 0.0f for static bodies
		float j_lin(const float& _e, const se::vec2& _velocity, const se::vec2& _normal, const float& _mass1, const float& _mass2);
		float j_rot(const float& _e, const se::vec2& _velocity, const se::vec2& _normal, const float& _mass1, const float& _mass2, 
			const se::vec2& _rVecAP, const se::vec2& _rVecBP, const float& _MoIA, const float& _MoIB);

		bool useGravity;

		se::CollisionResults collisionResults;
		std::vector<RigidBody2D*> bodies;
		se::vec2 gravity;
	};
}

