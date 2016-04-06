
#include "RigidBody2D.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Sprite.h"
#include "Exceptions.h"


namespace spehs
{
	RigidBody2D::RigidBody2D() : Component()
	{

	}
	RigidBody2D::RigidBody2D(GameObject* _owner) : Component(_owner)
	{
		isStatic = false;
		freezeRotation = false;
		useGravity = false;

		collider = CIRCLE;

		drag = 0.0f;
		angularDrag = 0.0f;
		mass = 0.0f;
		rotation = 0.0f;
		circleRadius = 0.0f;
		boxX = 0.0f;
		boxY = 0.0f;

		position = glm::vec2(0.0f);
		centerOfMass = glm::vec2(0.0f);
		velocity = glm::vec2(0.0f);
		acceleration = glm::vec2(0.0f);
		angularVelocity = glm::vec2(0.0f);
		maxAngularVelocity = glm::vec2(0.0f);
	}
	RigidBody2D::~RigidBody2D()
	{

	}


	void RigidBody2D::update()
	{
		//POST UPDATE?
		//If the GameObject has a transform component set it's values here
		if (ownerObject->getComponent<Transform2D>() != nullptr)
		{
			ownerObject->getComponent<Transform2D>()->setPosition(position);
			ownerObject->getComponent<Transform2D>()->setRotation(rotation);
		}
	}

	void RigidBody2D::applyForce(const glm::vec2& _force)
	{

	}

	void RigidBody2D::applyForceAtPosition(const glm::vec2& _force, const glm::vec2& _position)
	{

	}

	void RigidBody2D::applyImpulse(const glm::vec2& _impulse)
	{

	}

	void RigidBody2D::applyImpulseAtPosition(const glm::vec2& _force, const glm::vec2& _position)
	{

	}

	void RigidBody2D::applyTorque(const glm::vec2& _torque)
	{

	}

	void RigidBody2D::applyRelativeTorque(const glm::vec2& _torque)
	{

	}

	void RigidBody2D::setMass(const float& _newMass)
	{
		mass = _newMass;
	}

	void RigidBody2D::setSpriteCollider()
	{
		//The GameObject needs a sprite component
		if (ownerObject->getComponent<Sprite>() == nullptr)
			exceptions::fatalError("GameObject needs to have a sprite component for the RigidBody to use SpriteCollider");

		collider = SPRITE;
	}

	void RigidBody2D::setBoxCollider(const float& _width, const float& _height)
	{
		collider = BOX;
		boxX = _width;
		boxY = _height;
	}

	void RigidBody2D::setCircleCollider(const float& _radius)
	{
		collider = CIRCLE;
		circleRadius = _radius;
	}

	void RigidBody2D::setStatic(const bool _value)
	{
		isStatic = _value;
	}

	void RigidBody2D::setFreezeRotation(const bool _value)
	{
		freezeRotation = _value;
	}

	void RigidBody2D::setUseGravity(const bool _value)
	{
		useGravity = _value;
	}

	glm::vec2 RigidBody2D::getVelocity()
	{
		return velocity;
	}

	float RigidBody2D::getMass()
	{
		return mass;
	}

	Collider RigidBody2D::getCollider()
	{
		return collider;
	}
}