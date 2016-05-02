
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

#define DEFAULT_MASS_MULTIPLIER 1.0f
#define NULL_EPSILON 0.00001f


namespace spehs
{
	float projectForce(const glm::vec2& _force, const glm::vec2& _support)
	{
		if (glm::dot(_force, glm::normalize(_support)) < 0.0f)
		{
			return abs(glm::dot(_force, glm::normalize(_support)));
		}
		return 0.0f;
	}

	RigidBody2D::RigidBody2D() : Component()
	{
	}
	RigidBody2D::RigidBody2D(GameObject& _owner) : Component(_owner), world(nullptr)
	{
		isStatic = false;
		freezeRotation = false;
		freezePosition = false;
		useGravity = true;
		collider = SPRITE;

		circleRadius = 0.0f;
		boxX = 0.0f;
		boxY = 0.0f;

		drag = 10.0f;
		angularDrag = 10.0f;
		mass = 1.0f;
		rotation = 0.0f;
		elasticity = 0.6f;
		staticFriction = 1.0f;
		dynamicFriction = 0.01f;
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
		circleRadius = sprite->sprite->getRadius();
		calculateMOI();
		numVertices = sprite->sprite->worldVertexArray.size();
		vertexData = sprite->sprite->worldVertexArray.data();

		//Apply resultant force and torque
		acceleration = resultantForce / mass;
		angularAcceleration = resultantTorque / momentOfInertia;

		//Apply impulses
		if (resultantImpulseForce.size() && !freezePosition)
		{
			glm::vec2 resultForce(0.0f);
			for (unsigned i = 0; i < resultantImpulseForce.size(); i++)
				resultForce += resultantImpulseForce[i];
			resultForce /= resultantImpulseForce.size();
			velocity = resultForce;
		}
		if (resultantImpulseTorque.size() && !freezeRotation)
		{
			float resultTorque(0.0f);
			for (unsigned i = 0; i < resultantImpulseTorque.size(); i++)
				resultTorque += resultantImpulseTorque[i];
			resultTorque /= resultantImpulseTorque.size();
			angularVelocity = resultTorque;
		}

		//Apply acceleration
		if (!freezePosition)
			velocity += acceleration * getDeltaTime().asSeconds;
		if (!freezeRotation)
			angularVelocity += angularAcceleration * getDeltaTime().asSeconds;

		//For static bodies (NOTE: Doesn't work atm)
		if (isStatic)
			if (false)
		{
			velocity = transform->getLastSetPosition() - position;
			rotation = transform->getLastSetRotation() - rotation;
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

	void RigidBody2D::setFreezePosition(const bool _value)
	{
		freezePosition = _value;
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
		std::vector<Vertex> vArray = sprite->sprite->vertexArray;
		for (unsigned i = 0; i < vArray.size(); i++)
		{
			vArray[i].position.x *= sprite->sprite->width;
			vArray[i].position.y *= sprite->sprite->height;
		}

		//For regular convex polygons:
		for (unsigned i = 0; i < vArray.size(); i++)
		{
			if (i < vArray.size() - 1)
			{
				sum1 += abs(glm::length(glm::cross(toVec3(vArray[i + 1]), toVec3(vArray[i])))) *
					(
					(glm::dot(toVec3(vArray[i]), toVec3(vArray[i]))) +
					(glm::dot(toVec3(vArray[i]), toVec3(vArray[i + 1]))) +
					(glm::dot(toVec3(vArray[i + 1]), toVec3(vArray[i + 1])))
					);
				sum2 += abs(glm::length(glm::cross(toVec3(vArray[i + 1]), toVec3(vArray[i]))));
			}
			else if (i == vArray.size() - 1)
			{
				sum1 += abs(glm::length(glm::cross(toVec3(vArray[0]), toVec3(vArray[i])))) *
					(
					(glm::dot(toVec3(vArray[i]), toVec3(vArray[i]))) +
					(glm::dot(toVec3(vArray[i]), toVec3(vArray[0]))) +
					(glm::dot(toVec3(vArray[0]), toVec3(vArray[0])))
					);
				sum2 += abs(glm::length(glm::cross(toVec3(vArray[0]), toVec3(vArray[i]))));
			}
		}

		//Polygon
		momentOfInertia = (mass / 6.0f) * (sum1 / sum2);

		//Plane
		//momentOfInertia = (mass / 12.0f)*(sprite->sprite->width*sprite->sprite->width + sprite->sprite->height*sprite->sprite->height);

		//Circle
		//momentOfInertia = (mass * circleRadius * circleRadius) / 2.0f;
	}
}