
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "SATCollision.h"
#include "GameObject.h"
#include "Transform2D.h"

#include <glm/gtx/vector_query.hpp>


namespace spehs
{
	PhysicsWorld2D::PhysicsWorld2D() : gravity(0.0f, -9.81f), collisionPoint(nullptr)
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

	void PhysicsWorld2D::update()
	{
		for (unsigned cycle1 = 0; cycle1 < bodies.size(); cycle1++)
		{
			//Apply gravity
			if (!glm::isNull(gravity, 0.0001f))
			{
				if (!bodies[cycle1]->isStatic && bodies[cycle1]->useGravity)
					bodies[cycle1]->applyForce(gravity * bodies[cycle1]->getMass());
			}

			//Collision checking
			for (unsigned cycle2 = cycle1 + 1; cycle2 < bodies.size(); cycle2++)
			{
				//Radius collisions
				if (CircleCollision(bodies[cycle1]->position, bodies[cycle1]->circleRadius, bodies[cycle2]->position, bodies[cycle2]->circleRadius))
				{
					//SAT Collision
					if (bodies[cycle1]->numVertices > 10) //if circle
					{
						if (bodies[cycle2]->numVertices > 10) //both
						{
							collisionPoint = spehs::CircleMTVCollision(bodies[cycle1]->position, bodies[cycle1]->circleRadius, bodies[cycle2]->position, bodies[cycle2]->circleRadius);
						}
						else
						{
							collisionPoint = spehs::SATMTVCollision(bodies[cycle2]->vertexData, bodies[cycle2]->numVertices, bodies[cycle1]->position, bodies[cycle1]->circleRadius);
						}
					}
					else
					{
						if (bodies[cycle2]->numVertices > 10) //circle
						{
							collisionPoint = spehs::SATMTVCollision(bodies[cycle1]->vertexData, bodies[cycle1]->numVertices, bodies[cycle2]->position, bodies[cycle2]->circleRadius);
						}
						else
						{
							collisionPoint = spehs::SATMTVCollision(bodies[cycle1]->vertexData, bodies[cycle1]->numVertices, bodies[cycle2]->vertexData, bodies[cycle2]->numVertices);
						}
					}

					if (collisionPoint != nullptr)
					{
						glm::vec2 body1VelocityBefore = bodies[cycle1]->getVelocityAtPosition(collisionPoint->point);
						glm::vec2 body2VelocityBefore = bodies[cycle2]->getVelocityAtPosition(collisionPoint->point);
						float relativeNormalVelocity = glm::dot((body1VelocityBefore - body2VelocityBefore), collisionPoint->MTV);

						//Separate
						float body1Percentage = glm::length(body1VelocityBefore) / (glm::length(body1VelocityBefore) + glm::length(body2VelocityBefore));
						if (body1Percentage != body1Percentage) //if 2 zero-velocity bodies collide
							body1Percentage = 0.5f;
						if (glm::dot(body1VelocityBefore, collisionPoint->MTV) > 0.0f)
						{
							bodies[cycle1]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle1]->ownerObject->getComponent<Transform2D>()->getPosition() - collisionPoint->MTV * body1Percentage);
							bodies[cycle2]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle2]->ownerObject->getComponent<Transform2D>()->getPosition() + collisionPoint->MTV * (1.0f - body1Percentage));
						}
						else
						{
							bodies[cycle1]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle1]->ownerObject->getComponent<Transform2D>()->getPosition() + collisionPoint->MTV * body1Percentage);
							bodies[cycle2]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle2]->ownerObject->getComponent<Transform2D>()->getPosition() - collisionPoint->MTV * (1.0f - body1Percentage));
						}

						glm::vec2 body1VelocityAfter;
						glm::vec2 body2VelocityAfter;
						float e = (bodies[cycle1]->elasticity + bodies[cycle2]->elasticity) / 2.0f;
						//Resolve collisions
						if (relativeNormalVelocity < 0.0f) //Point are colliding
						{
							if (bodies[cycle1]->isStatic)
							{
								if (bodies[cycle2]->isStatic)
								{
									body1VelocityAfter = body1VelocityBefore;
									body2VelocityAfter = body2VelocityBefore;
								}
								else
								{
									body1VelocityAfter = body1VelocityBefore;
									body2VelocityAfter = body2VelocityBefore - (j(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->MTV, 0.0f, bodies[cycle2]->mass) / bodies[cycle2]->mass) * collisionPoint->MTV;
								}
							}
							else if (bodies[cycle2]->isStatic)
							{
								body1VelocityAfter = body1VelocityBefore + (j(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->MTV, bodies[cycle1]->mass, 0.0f) / bodies[cycle1]->mass) * collisionPoint->MTV;
								body2VelocityAfter = body2VelocityBefore;
							}
							else
							{
								body1VelocityAfter = body1VelocityBefore + (j(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->MTV, bodies[cycle1]->mass, bodies[cycle2]->mass) / bodies[cycle1]->mass) * collisionPoint->MTV;
								body2VelocityAfter = body2VelocityBefore - (j(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->MTV, bodies[cycle1]->mass, bodies[cycle2]->mass) / bodies[cycle2]->mass) * collisionPoint->MTV;
							}

							bodies[cycle1]->applyVelocityImpulse(body1VelocityAfter);
							bodies[cycle2]->applyVelocityImpulse(body2VelocityAfter);
						}
						else if (relativeNormalVelocity == 0.0f) //Points are in contact
						{
							//TODO: sick contact fysicks
						}
						else //Points are separating
						{
							//Ignore
							int i = 0;
						}

						delete collisionPoint;
						collisionPoint = nullptr;
					}
				}
			}
		}
	}

	void PhysicsWorld2D::setGravity(const glm::vec2& _gravity)
	{
		gravity = _gravity;
	}

	float PhysicsWorld2D::j(const float& _e, const glm::vec2& _velocity, const glm::vec2& _normal, const float& _mass1, const float& _mass2)
	{
		if (_mass1 == 0.0f) //if 1 is static
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / glm::dot(_normal, _normal*(1/_mass2));
		}
		else if (_mass2 == 0.0f) //if 2 is static
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / glm::dot(_normal, _normal*(1 / _mass1));
		}
		else
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / glm::dot(_normal, _normal*((1 / _mass1) + (1 / _mass2)));
		}
	}
}