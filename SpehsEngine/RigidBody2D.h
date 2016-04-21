
#pragma once

#include "Component.h"

#include <glm/vec2.hpp>

#include <vector>
#include <algorithm>


namespace spehs
{
	class GameObject;
	class PhysicsWorld2D;
	class Vertex;

	enum Collider : char
	{
		CIRCLE,
		BOX,
		SPRITE,
	};
	
	class RigidBody2D : public Component
	{
		friend PhysicsWorld2D;
	public:
		RigidBody2D();
		RigidBody2D(GameObject& _owner);
		~RigidBody2D();

		void update();

		void applyForce(const glm::vec2& _force);
		void applyForceAtPosition(const glm::vec2& _force, const glm::vec2& _position);
		void applyTorque(const float& _torque);
		void applyVelocityImpulse(const glm::vec2& _impulse);
		void applyAngularImpulse(const float& _impulse);
		void applySupportForce(const glm::vec2& _normal, const glm::vec2& _position);

		//Setters
		void setMass(const float& _newMass);
		void setSpriteCollider();
		void setBoxCollider(const float& _width, const float& _height);
		void setCircleCollider(const float& _radius);
		void setStatic(const bool _value);
		void setFreezeRotation(const bool _value);
		void setUseGravity(const bool _value);
		void setDrag(const float& _drag, const float& _angularDrag);
		void setElasticity(const float& _e);

		//Getters
		glm::vec2 getVelocityAtPosition(const glm::vec2& _position);
		float getMass() const { return mass; }

	private:
		bool setWorld(PhysicsWorld2D* _world);
		PhysicsWorld2D* world;

		void calculateMOI();

		bool isStatic;
		bool freezeRotation;
		bool useGravity;

		Collider collider;
		float circleRadius;
		float boxX, boxY;
		int numVertices;
		Vertex* vertexData;

		float rotation;
		float mass;
		float elasticity; //Coefficient of Restitution
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
		std::vector<std::pair<glm::vec2, glm::vec2>> supportForces;
	};
}

