
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Sprite.h"
#include "Exceptions.h"
#include "Time.h"
#include "Polygon.h"


namespace spehs
{
	RigidBody2D::RigidBody2D() : Component()
	{
	}
	RigidBody2D::RigidBody2D(GameObject& _owner) : Component(_owner), world(nullptr), CENTER(0.0f, 0.0f)
	{
		isStatic = false;
		freezeRotation = false;
		useGravity = true;
		collider = SPRITE;

		circleRadius = 0.0f;
		boxX = 0.0f;
		boxY = 0.0f;

		drag = 0.1f;
		angularDrag = 0.1f;
		mass = 1.0f;
		rotation = 0.0f;
		elasticity = 0.5f;
		angularVelocity = 0.01f;
		angularAcceleration = 0.0f;

		position = glm::vec2(0.0f);
		centerOfMass = glm::vec2(0.0f);
		velocity = glm::vec2(-1.0f);
		acceleration = glm::vec2(0.0f);

		update();
	}
	RigidBody2D::~RigidBody2D()
	{
		if (world)
			world->removeRigidBody(*this);
	}

	void RigidBody2D::update()
	{
		Transform2D* transform = ownerObject->getComponent<Transform2D>();
		if (!transform)
			exceptions::fatalError("Object doesn't have transform component!");

		//Apply forces
		//>>

		//Apply impulses
		//>>

		//Apply acceleration
		velocity += acceleration * getDeltaTime().asSeconds;
		angularVelocity += angularAcceleration * getDeltaTime().asSeconds;

		//Update transform based on velocity
		transform->setPosition(transform->getPosition() + velocity);
		transform->setRotation(transform->getRotation() + angularVelocity);

		//Update RigidBodys position and rotation from transform
		position = transform->getPosition();
		rotation = transform->getRotation();
		//Update radius, vertices
		circleRadius = ownerObject->getComponent<Sprite>()->sprite->getRadius();
		numVertices = ownerObject->getComponent<Sprite>()->sprite->worldVertexArray.size();
		vertexData = ownerObject->getComponent<Sprite>()->sprite->worldVertexArray.data();

		//Apply drag
		//if (velocity.x > drag)
		//	velocity.x -= drag * getDeltaTime().asSeconds;
		//else if (velocity.x > 0.0f)
		//	velocity.x = 0.0f;
		//if (velocity.y > drag)
		//	velocity.y -= drag * getDeltaTime().asSeconds;
		//else if (velocity.y > 0.0f)
		//	velocity.y = 0.0f;

		//angularVelocity -= angularDrag * getDeltaTime().asSeconds;
	}

	void RigidBody2D::applyForce(const glm::vec2& _force)
	{
		applyForceAtPosition(_force, CENTER);
	}

	void RigidBody2D::applyForceAtPosition(const glm::vec2& _force, const glm::vec2& _position)
	{

	}

	void RigidBody2D::applyImpulse(const glm::vec2& _impulse)
	{
		applyImpulseAtPosition(_impulse, CENTER);
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
		exceptions::fatalError("Collision mode not supported at the moment!");
		collider = BOX;
		boxX = _width;
		boxY = _height;
	}

	void RigidBody2D::setCircleCollider(const float& _radius)
	{
		exceptions::fatalError("Collision mode not supported at the moment!");
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

	void RigidBody2D::setDrag(const float& _drag, const float& _angularDrag)
	{
		drag = _drag;
		angularDrag = _angularDrag;
	}

	void RigidBody2D::setElasticity(const float& _e)
	{
		elasticity = _e;

		//restrict to 0-1
		if (elasticity > 1.0f)
			elasticity = 1.0f;
		else if (elasticity < 0.0f)
			elasticity = 0.0f;
	}

	float RigidBody2D::getMass()
	{
		return mass;
	}

	Collider RigidBody2D::getCollider()
	{
		return collider;
	}

	bool RigidBody2D::setWorld(PhysicsWorld2D* _world)
	{
		if (_world == nullptr)
		{
			world->removeRigidBody(*this);
			world = nullptr;
			return true;
		}
		if (world)
			return false;
		world = _world;
		return true;
	}
}