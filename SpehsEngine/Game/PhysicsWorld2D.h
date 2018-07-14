
#pragma once

#include "SpehsEngine/Physics/SATCollision.h"
#include <glm/vec2.hpp>

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
		void setGravity(const glm::vec2& _gravity);

		//Getters
		glm::vec2 getGravity() const{ return gravity; }

	private:
		//Enter mass as 0.0f for static bodies
		float j_lin(const float& _e, const glm::vec2& _velocity, const glm::vec2& _normal, const float& _mass1, const float& _mass2);
		float j_rot(const float& _e, const glm::vec2& _velocity, const glm::vec2& _normal, const float& _mass1, const float& _mass2, 
			const glm::vec2& _rVecAP, const glm::vec2& _rVecBP, const float& _MoIA, const float& _MoIB);

		bool useGravity = true;

		se::CollisionResults collisionResults;
		std::vector<RigidBody2D*> bodies;
		glm::vec2 gravity = glm::vec2(0.0f, -3.0f);
	};
}

