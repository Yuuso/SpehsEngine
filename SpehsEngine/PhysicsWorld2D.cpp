#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"


namespace spehs
{
	PhysicsWorld2D::PhysicsWorld2D()
	{
	}
	PhysicsWorld2D::~PhysicsWorld2D()
	{
		for (unsigned i = 0; i < bodies.size(); i++)
		{
			bodies[i]->setWorld(nullptr);
		}
	}

	bool PhysicsWorld2D::addRigidBody(RigidBody2D& _body)
	{
		if (_body.setWorld(this))
		{
			bodies.push_back(&_body);
			return true;
		}
		return false;
	}
	void PhysicsWorld2D::removeRigidBody(RigidBody2D& body)
	{
		for (unsigned i = 0; i < bodies.size(); i++)
		{
			if (bodies[i] == &body)
			{
				bodies[i] = bodies.back();
				bodies.pop_back();
				return;
			}
		}
	}
}