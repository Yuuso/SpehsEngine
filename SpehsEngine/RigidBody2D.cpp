
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

#include <glm/gtx/vector_query.hpp>

#define DEFAULT_MASS_MULTIPLIER 20.0f
#define NULL_EPSILON 0.00001f


namespace spehs
{
	RigidBody2D::RigidBody2D() : Component()
	{
	}
	RigidBody2D::RigidBody2D(GameObject& _owner) : Component(_owner), world(nullptr)
	{
		isStatic = false;
		freezeRotation = false;
		useGravity = true;
		collider = SPRITE;

		circleRadius = 0.0f;
		boxX = 0.0f;
		boxY = 0.0f;

		drag = 10.0f;
		angularDrag = 10.0f;
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
		mass = sprite->sprite->getArea() * DEFAULT_MASS_MULTIPLIER;
		centerOfMass = getCenter(sprite->sprite->worldVertexArray.data(), sprite->sprite->worldVertexArray.size());
		calculateMOI();
		circleRadius = sprite->sprite->getRadius();
		numVertices = sprite->sprite->worldVertexArray.size();
		vertexData = sprite->sprite->worldVertexArray.data();

		//Apply support forces
		for (unsigned i = 0; i < supportForces.size(); i++)
		{

		}

		//Apply resultant force and torque
		acceleration = resultantForce / mass;
		angularAcceleration = resultantTorque / momentOfInertia;

		//Apply acceleration
		velocity += acceleration * getDeltaTime().asSeconds;
		angularVelocity += angularAcceleration * getDeltaTime().asSeconds;

		//Apply impulses
		if (resultantImpulseForce.size())
		{
			glm::vec2 resultForce(0.0f);
			for (unsigned i = 0; i < resultantImpulseForce.size(); i++)
				resultForce += resultantImpulseForce[i];
			resultForce /= resultantImpulseForce.size();
			velocity = resultForce;
		}
		if (resultantImpulseTorque.size())
		{
			float resultTorque(0.0f);
			for (unsigned i = 0; i < resultantImpulseTorque.size(); i++)
				resultTorque += resultantImpulseTorque[i];
			resultTorque /= resultantImpulseTorque.size();
			angularVelocity = resultTorque;
		}

		//For static bodies (NOTE: Doesn't work atm)
		if (isStatic)
			if (false)
		{
			velocity = transform->getLastSetPosition() - transform->getPosition();
			rotation = transform->getLastSetRotation() - transform->getRotation();
		}

		//Update transform based on velocity
		transform->setPosition(transform->getPosition() + velocity);
		transform->setRotation(transform->getRotation() + angularVelocity);

		//Update RigidBodys position and rotation from transform
		position = transform->getPosition();
		rotation = transform->getRotation();

		//Reset forces
		resultantForce = glm::vec2(0.0f);
		resultantTorque = 0.0f;
		resultantImpulseForce.clear();
		resultantImpulseTorque.clear();
		supportForces.clear();

		//Apply drag
		if (!glm::isNull(velocity, NULL_EPSILON))
			applyForce(-glm::normalize(velocity) * drag);
		if (abs(angularVelocity) > NULL_EPSILON)
			applyTorque(-(angularVelocity / abs(angularVelocity)) * angularDrag);
	}

	void RigidBody2D::applyForce(const glm::vec2& _force)
	{
		applyForceAtPosition(_force, centerOfMass);
	}

	void RigidBody2D::applyForceAtPosition(const glm::vec2& _force, const glm::vec2& _position)
	{
		resultantForce += _force;
		glm::vec2 AtoB = glm::vec2(_position - centerOfMass);
		applyTorque(cross2(AtoB, _force));
	}

	void RigidBody2D::applyVelocityImpulse(const glm::vec2& _impulse)
	{
		resultantImpulseForce.push_back(_impulse);
	}

	void RigidBody2D::applyAngularImpulse(const float& _impulse)
	{
		resultantImpulseTorque.push_back(_impulse);
	}

	void RigidBody2D::applyTorque(const float& _torque)
	{
		resultantTorque += _torque;
	}

	void RigidBody2D::applySupportForce(const glm::vec2& _normal, const glm::vec2& _position)
	{
		supportForces.push_back({ _normal, _position });
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
		for (unsigned i = 0; i < sprite->sprite->vertexArray.size(); i++)
		{
			if (i < sprite->sprite->vertexArray.size() - 1)
			{
				sum1 += abs(glm::length(glm::cross(toVec3(sprite->sprite->vertexArray[i + 1]), toVec3(sprite->sprite->vertexArray[i])))) *
					(
					(glm::dot(toVec3(sprite->sprite->vertexArray[i]), toVec3(sprite->sprite->vertexArray[i]))) +
					(glm::dot(toVec3(sprite->sprite->vertexArray[i]), toVec3(sprite->sprite->vertexArray[i + 1]))) +
					(glm::dot(toVec3(sprite->sprite->vertexArray[i + 1]), toVec3(sprite->sprite->vertexArray[i + 1])))
					);
				sum2 += abs(glm::length(glm::cross(toVec3(sprite->sprite->vertexArray[i + 1]), toVec3(sprite->sprite->vertexArray[i]))));
			}
			else if (i == sprite->sprite->vertexArray.size() - 1)
			{
				sum1 += abs(glm::length(glm::cross(toVec3(sprite->sprite->vertexArray[0]), toVec3(sprite->sprite->vertexArray[i])))) *
					(
					(glm::dot(toVec3(sprite->sprite->vertexArray[i]), toVec3(sprite->sprite->vertexArray[i]))) +
					(glm::dot(toVec3(sprite->sprite->vertexArray[i]), toVec3(sprite->sprite->vertexArray[0]))) +
					(glm::dot(toVec3(sprite->sprite->vertexArray[0]), toVec3(sprite->sprite->vertexArray[0])))
					);
				sum2 += abs(glm::length(glm::cross(toVec3(sprite->sprite->vertexArray[0]), toVec3(sprite->sprite->vertexArray[i]))));
			}
		}

		momentOfInertia = (mass / 6) * (sum1 / sum2);
	}
}