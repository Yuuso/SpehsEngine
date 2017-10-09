
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Collider.h"
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
	float projectForce(const spehs::vec2& _force, const spehs::vec2& _support)
	{
		if (_force.dot(_support.getNormalized()) < 0.0f)
		{
			return abs(_force.dot(_support.getNormalized()));
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

		position = spehs::vec2::zero;
		centerOfMass = spehs::vec2::zero;
		velocity = spehs::vec2::zero;
		acceleration = spehs::vec2::zero;

		resultantTorque = 0.0f;
		resultantForce = spehs::vec2::zero;

		update(0);
	}
	RigidBody2D::~RigidBody2D()
	{
		if (world)
			world->removeRigidBody(*this);
	}

	void RigidBody2D::update(const time::Time deltaTime)
	{
		Transform2D* transform = ownerObject->getComponent<Transform2D>();
		if (!transform)
			exceptions::fatalError("Object doesn't have transform component!");
		Collider* collider = ownerObject->getComponent<Collider>();
		if (!collider)
			exceptions::fatalError("Object doesn't have sprite component!");

		//Update from sprite (this really needs to be done only when the collider has changed)
		mass = collider->collisionPolygon->getArea() * DEFAULT_MASS_MULTIPLIER;
		centerOfMass = getCenter(collider->collisionPolygon->worldVertexArray.data(), collider->collisionPolygon->worldVertexArray.size());
		circleRadius = collider->collisionPolygon->getRadius();
		calculateMOI();
		numVertices = collider->collisionPolygon->worldVertexArray.size();
		vertexData = collider->collisionPolygon->worldVertexArray.data();

		//Apply resultant force and torque
		acceleration = resultantForce / mass;
		angularAcceleration = resultantTorque / momentOfInertia;

		//Apply impulses
		if (resultantImpulseForce.size() && !freezePosition)
		{
			spehs::vec2 resultForce = spehs::vec2::zero;
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
			velocity += acceleration * deltaTime.asSeconds();
		if (!freezeRotation)
			angularVelocity += angularAcceleration * deltaTime.asSeconds();

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
		resultantForce = spehs::vec2::zero;
		resultantTorque = 0.0f;
		resultantImpulseForce.clear();
		resultantImpulseTorque.clear();

		//Apply drag
		if (!velocity.isNull(NULL_EPSILON))
			applyForce(-(velocity.getNormalized()) * drag);
		if (abs(angularVelocity) > NULL_EPSILON)
			applyTorque(-(angularVelocity / abs(angularVelocity)) * angularDrag);
	}

	void RigidBody2D::applyForce(const spehs::vec2& _force)
	{
		applyForceAtPosition(_force, centerOfMass);
	}

	void RigidBody2D::applyForceAtPosition(const spehs::vec2& _force, const spehs::vec2& _position)
	{
		resultantForce += _force;
		spehs::vec2 AtoB = spehs::vec2(_position - centerOfMass);
		applyTorque(cross2(AtoB, _force));
	}

	void RigidBody2D::applyVelocityImpulse(const spehs::vec2& _impulse)
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
	
	spehs::vec2 RigidBody2D::getVelocityAtPosition(const spehs::vec2& _position)
	{
		//Angular Velocity
		spehs::vec2 result = _position - centerOfMass;
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
		Collider* collider = ownerObject->getComponent<Collider>();
		std::vector<Vertex> vArray = collider->collisionPolygon->vertexArray;
		for (unsigned i = 0; i < vArray.size(); i++)
		{
			vArray[i].position.x *= collider->collisionPolygon->width;
			vArray[i].position.y *= collider->collisionPolygon->height;
		}

		//For regular convex polygons:
		for (unsigned i = 0; i < vArray.size(); i++)
		{
			if (i < vArray.size() - 1)
			{
				sum1 += abs(toVec3(vArray[i + 1]).cross(toVec3(vArray[i])).getLength()) *
					(
					toVec3(vArray[i]).dot(toVec3(vArray[i])) +
					toVec3(vArray[i]).dot(toVec3(vArray[i + 1])) +
					toVec3(vArray[i + 1]).dot(toVec3(vArray[i + 1]))
					);
				sum2 += abs(toVec3(vArray[i + 1]).cross(toVec3(vArray[i])).getLength());
			}
			else if (i == vArray.size() - 1)
			{
				sum1 += abs((toVec3(vArray[0]).cross(toVec3(vArray[i])).getLength())) *
					(
					toVec3(vArray[i]).dot(toVec3(vArray[i])) +
					toVec3(vArray[i]).dot(toVec3(vArray[0])) +
					toVec3(vArray[0]).dot(toVec3(vArray[0]))
					);
				sum2 += abs(toVec3(vArray[0]).cross(toVec3(vArray[i])).getLength());
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