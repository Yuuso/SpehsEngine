
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Sprite.h"
#include "Exceptions.h"
#include "Time.h"
#include "Polygon.h"
#include "Geometry.h"
#include "Vertex.h"
#include "Arrow.h"

#include <glm/gtx/vector_query.hpp>


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
		momentOfInertia = 0.0f;
		angularVelocity = 0.0f;
		angularAcceleration = 0.0f;

		position = glm::vec2(0.0f);
		centerOfMass = glm::vec2(0.0f);
		velocity = glm::vec2(0.0f);
		acceleration = glm::vec2(0.0f);

		resultantTorque = 0.0f;
		resultantForce = glm::vec2(0.0f);
		resultantImpulseTorque = 0.0f;
		resultantImpulseForce = glm::vec2(0.0f);

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
		Sprite* sprite = ownerObject->getComponent<Sprite>();
		if (!sprite)
			exceptions::fatalError("Object doesn't have sprite component!");

		//Update from sprite (this really needs to be done only when the sprite has changed)
		mass = sprite->sprite->getArea();
		centerOfMass = getCenter(sprite->sprite->worldVertexArray.data(), sprite->sprite->worldVertexArray.size());
		calculateMOI();
		circleRadius = sprite->sprite->getRadius();
		numVertices = sprite->sprite->worldVertexArray.size();
		vertexData = sprite->sprite->worldVertexArray.data();

		//Apply resultant force and torque
		acceleration = resultantForce / mass;
		angularAcceleration = resultantTorque / momentOfInertia;

		//Apply resultant impulse
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

		//Reset forces
		resultantForce = glm::vec2(0.0f);
		resultantTorque = 0.0f;
		resultantImpulseForce = glm::vec2(0.0f);
		resultantImpulseTorque = 0.0f;

		//Apply drag
		if (!glm::isNull(velocity, 0.0001f))
			applyForce(-glm::normalize(velocity) * drag);
		if (abs(angularVelocity) > 0.0001f)
			applyTorque(-(angularVelocity / abs(angularVelocity)) * angularDrag);
	}

	void RigidBody2D::applyForce(const glm::vec2& _force)
	{
		applyForceAtPosition(_force, CENTER);
	}

	void RigidBody2D::applyForceAtPosition(const glm::vec2& _force, const glm::vec2& _position)
	{
		if (SHOW_FORCES)
		{
			forces.push_back(new Arrow(_position, _position + _force));
			forces.back()->setArrowColor(0, 0, 255, 255);
		}

		resultantForce += _force;
		glm::vec2 AtoB = glm::vec2(_position - centerOfMass);
		glm::vec2 rAB = glm::vec2(-AtoB.y, AtoB.x);
		applyTorque(glm::dot(rAB, _force));
		if (SHOW_FORCES)
		{
			forces.push_back(new Arrow(_position, _position + rAB));
			forces.back()->setArrowColor(0, 255, 0, 255);
		}
	}

	void RigidBody2D::applyImpulse(const glm::vec2& _impulse)
	{
		applyImpulseAtPosition(_impulse, CENTER);
	}

	void RigidBody2D::applyImpulseAtPosition(const glm::vec2& _force, const glm::vec2& _position)
	{
		if (SHOW_FORCES)
		{
			forces.push_back(new Arrow(_position, _position + _force));
			forces.back()->setArrowColor(255, 0, 0, 255);
		}

	}

	void RigidBody2D::applyTorque(const float& _torque)
	{
		resultantTorque += _torque;
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
	
	glm::vec2 RigidBody2D::getVelocityAtPosition(const glm::vec2& _position)
	{
		//Angular Velocity
		glm::vec2 result = _position - centerOfMass;
		float x = result.x;
		result.x = -result.y;
		result.y = x;

		result *= angularVelocity;

		//Linear velocity
		result += velocity;

		return result;
	}

	//Private:
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

	void RigidBody2D::calculateMOI()
	{
		float sum1 = 0.0f, sum2 = 0.0f;
		Sprite* sprite = ownerObject->getComponent<Sprite>();
		
		//For regular convex polygons:
		for (unsigned i = 0; i < sprite->sprite->worldVertexArray.size(); i++)
		{
			if (i < sprite->sprite->worldVertexArray.size() - 1)
			{
				sum1 += abs(glm::length(glm::cross(toVec3(sprite->sprite->worldVertexArray[i + 1]), toVec3(sprite->sprite->worldVertexArray[i])))) *
					(
					(glm::dot(toVec3(sprite->sprite->worldVertexArray[i]), toVec3(sprite->sprite->worldVertexArray[i]))) +
					(glm::dot(toVec3(sprite->sprite->worldVertexArray[i]), toVec3(sprite->sprite->worldVertexArray[i + 1]))) +
					(glm::dot(toVec3(sprite->sprite->worldVertexArray[i + 1]), toVec3(sprite->sprite->worldVertexArray[i + 1])))
					);
				sum2 += abs(glm::length(glm::cross(toVec3(sprite->sprite->worldVertexArray[i + 1]), toVec3(sprite->sprite->worldVertexArray[i]))));
			}
			else if (i == sprite->sprite->worldVertexArray.size() - 1)
			{
				sum1 += abs(glm::length(glm::cross(toVec3(sprite->sprite->worldVertexArray[0]), toVec3(sprite->sprite->worldVertexArray[i])))) *
					(
					(glm::dot(toVec3(sprite->sprite->worldVertexArray[i]), toVec3(sprite->sprite->worldVertexArray[i]))) +
					(glm::dot(toVec3(sprite->sprite->worldVertexArray[i]), toVec3(sprite->sprite->worldVertexArray[0]))) +
					(glm::dot(toVec3(sprite->sprite->worldVertexArray[0]), toVec3(sprite->sprite->worldVertexArray[0])))
					);
				sum2 += abs(glm::length(glm::cross(toVec3(sprite->sprite->worldVertexArray[0]), toVec3(sprite->sprite->worldVertexArray[i]))));
			}
		}

		momentOfInertia = (mass / 6) * (sum1 / sum2);
	}
}