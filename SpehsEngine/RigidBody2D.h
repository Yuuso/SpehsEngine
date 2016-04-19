
#pragma once

#include "Component.h"

#include <glm/vec2.hpp>

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
		void applyImpulse(const glm::vec2& _impulse);
		void applyImpulseAtPosition(const glm::vec2& _force, const glm::vec2& _position);
		void applyTorque(const glm::vec2& _torque);
		void applyRelativeTorque(const glm::vec2& _torque);

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
		float getMass();
		Collider getCollider();

	private:
		bool setWorld(PhysicsWorld2D* _world);
		PhysicsWorld2D* world;

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
		float elasticity;
		float drag;
		float angularDrag;
		float angularVelocity;
		float angularAcceleration;

		glm::vec2 position;
		glm::vec2 centerOfMass;
		glm::vec2 velocity;
		glm::vec2 acceleration;

		//Vectors of forces, torques and impulses ?

		const glm::vec2 CENTER;
	};
}

