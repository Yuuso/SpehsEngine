
#pragma once

#include "SpehsEngine/Game/Component.h"

#include <vector>
#include <algorithm>
#include <glm/vec2.hpp>


namespace se
{
	class GameObject;
	class PhysicsWorld2D;
	class Vertex;

	class RigidBody2D : public Component
	{
		friend PhysicsWorld2D;
	public:
		RigidBody2D();
		RigidBody2D(GameObject& _owner);
		~RigidBody2D();

		void update(const time::Time deltaTime) override;

		void applyForce(const glm::vec2& _force);
		void applyForceAtPosition(const glm::vec2& _force, const glm::vec2& _position);
		void applyTorque(const float& _torque);
		void applyVelocityImpulse(const glm::vec2& _impulse);
		void applyAngularImpulse(const float& _impulse);

		//Setters
		void setMass(const float& _newMass);
		void setStatic(const bool _value);
		void setFreezeRotation(const bool _value);
		void setFreezePosition(const bool _value);
		void setUseGravity(const bool _value);
		void setDrag(const float& _drag, const float& _angularDrag);
		void setElasticity(const float& _e);

		//Getters
		glm::vec2 getVelocityAtPosition(const glm::vec2& _position);
		float getMass() const { return mass; }
		float getInvMass() const { if (isStatic || freezePosition) return 0.0f; else return 1 / mass; }
		float getInvMoI() const { if (isStatic || freezeRotation) return 0.0f; else return 1 / momentOfInertia; }
		bool getIsStatic() const { return isStatic; }
		bool getFreezeRotation() const { return freezeRotation; }
		bool getFreezePosition() const { return freezePosition; }

	private:
		bool setWorld(PhysicsWorld2D* _world);
		PhysicsWorld2D* world;

		void calculateMOI();

		bool isStatic;
		bool freezeRotation;
		bool freezePosition;
		bool useGravity;

		float circleRadius;
		float boxX, boxY;
		size_t numVertices;
		Vertex* vertexData;

		float rotation;
		float mass;
		float elasticity; //Coefficient of Restitution
		float staticFriction;
		float dynamicFriction;
		float momentOfInertia;
		float drag;
		float angularDrag;
		float angularVelocity;
		float angularAcceleration;

		glm::vec2 position;
		glm::vec2 centerOfMass;
		glm::vec2 velocity;
		glm::vec2 acceleration;

		float resultantTorque;
		glm::vec2 resultantForce;
		std::vector<float> resultantImpulseTorque;
		std::vector<glm::vec2> resultantImpulseForce;
	};
}

