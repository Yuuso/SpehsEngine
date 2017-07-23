
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "SATCollision.h"
#include "GameObject.h"
#include "Transform2D.h"
#include "Geometry.h"
#include "Vector.h"

#include "Sprite.h"
#include "Polygon.h"
#include "Line.h"
#include "BatchManager.h"

#include <glm/gtx/vector_query.hpp>

#define ZERO_EPSILON 0.000001f

#define SHOW_COLORS false


namespace spehs
{
	PhysicsWorld2D::PhysicsWorld2D() : gravity(0.0f, -3.0f), collisionPoint(nullptr), useGravity(true)
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
			if (useGravity)
			{
				if (!gravity.isNull(ZERO_EPSILON))
				{
					if (!bodies[cycle1]->isStatic && bodies[cycle1]->useGravity)
						bodies[cycle1]->applyForce(gravity * bodies[cycle1]->getMass());
				}
			}

			//Collision checking
			for (unsigned cycle2 = cycle1 + 1; cycle2 < bodies.size(); cycle2++)
			{
				//if (bodies[cycle1] == bodies[cycle2])
				//	continue;

				//Radius collisions
				if (circleCollision(bodies[cycle1]->position, bodies[cycle1]->circleRadius, bodies[cycle2]->position, bodies[cycle2]->circleRadius))
				{
					//SAT Collision
					if (bodies[cycle1]->numVertices > 10) //if circle
					{
						if (bodies[cycle2]->numVertices > 10) //both
						{
							collisionPoint = spehs::circleMTVCollision(bodies[cycle1]->position, bodies[cycle1]->circleRadius, bodies[cycle2]->position, bodies[cycle2]->circleRadius);
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
						for (unsigned i = 0; i < collisionPoint->point.size(); i++)
						{
							spehs::vec2 body1VelocityBefore = bodies[cycle1]->getVelocityAtPosition(collisionPoint->point[i]);
							spehs::vec2 body2VelocityBefore = bodies[cycle2]->getVelocityAtPosition(collisionPoint->point[i]);
							spehs::vec2 relativeVelocity = (body1VelocityBefore - body2VelocityBefore);
							float relativeNormalVelocity = relativeVelocity.dot(collisionPoint->normal[i]);

							//Resolve collisions
							if (relativeNormalVelocity < -ZERO_EPSILON) //Point are colliding
							{
								//DEbug colors
								if (SHOW_COLORS)
								{
									bodies[cycle1]->ownerObject->getComponent<Sprite>()->setColor(spehs::Color(255,0,0,255));
									bodies[cycle2]->ownerObject->getComponent<Sprite>()->setColor(spehs::Color(255,0,0,255));
								}

								//Positional Correction
								const float percentage = 0.25f;
								const float slop = 0.04f;
								spehs::vec2 correction = std::max(collisionPoint->MTV.getLength() - slop, 0.0f) / (bodies[cycle1]->getInvMass() + bodies[cycle2]->getInvMass()) * percentage * collisionPoint->normal[i];
								if (!bodies[cycle1]->freezePosition && !bodies[cycle1]->isStatic)
									bodies[cycle1]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle1]->ownerObject->getComponent<Transform2D>()->getPosition() + correction * bodies[cycle1]->getInvMass());
								if (!bodies[cycle2]->freezePosition && !bodies[cycle2]->isStatic)
									bodies[cycle2]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle2]->ownerObject->getComponent<Transform2D>()->getPosition() - correction * bodies[cycle2]->getInvMass());

								//Impulses
								spehs::vec2 body1VelocityAfter;
								spehs::vec2 body2VelocityAfter;
								float body1AngularVelocityAfter;
								float body2AngularVelocityAfter;
								spehs::vec2 body1LinearFrictionAfter;
								spehs::vec2 body2LinearFrictionAfter;
								float body1AngularFrictionAfter;
								float body2AngularFrictionAfter;
								
								float e = std::min(bodies[cycle1]->elasticity, bodies[cycle2]->elasticity);

								spehs::vec2 rVecAP = spehs::vec2(-(collisionPoint->point[i] - bodies[cycle1]->centerOfMass).y, (collisionPoint->point[i] - bodies[cycle1]->centerOfMass).x);
								spehs::vec2 rVecBP = spehs::vec2(-(collisionPoint->point[i] - bodies[cycle2]->centerOfMass).y, (collisionPoint->point[i] - bodies[cycle2]->centerOfMass).x);
								spehs::vec2 tangent = spehs::vec2(-collisionPoint->normal[i].y, collisionPoint->normal[i].x);
								if (tangent.dot(relativeVelocity) < 0.0f)
									tangent = -tangent;

								float jL = j_lin(e, relativeVelocity, collisionPoint->normal[i], bodies[cycle1]->getInvMass(), bodies[cycle2]->getInvMass());
								float jR = j_rot(e, relativeVelocity, collisionPoint->normal[i], bodies[cycle1]->getInvMass(), bodies[cycle2]->getInvMass(), rVecAP, rVecBP, bodies[cycle1]->getInvMoI(), bodies[cycle2]->getInvMoI());
								float jLt = j_lin(e, relativeVelocity, tangent, bodies[cycle1]->getInvMass(), bodies[cycle2]->getInvMass());
								float jRt = j_rot(e, relativeVelocity, tangent, bodies[cycle1]->getInvMass(), bodies[cycle2]->getInvMass(), rVecAP, rVecBP, bodies[cycle1]->getInvMoI(), bodies[cycle2]->getInvMoI());

								//Collision impulses
								body1VelocityAfter = body1VelocityBefore + (jL * bodies[cycle1]->getInvMass()) * collisionPoint->normal[i];
								body2VelocityAfter = body2VelocityBefore + (-jL * bodies[cycle2]->getInvMass()) * collisionPoint->normal[i];

								body1AngularVelocityAfter = bodies[cycle1]->angularVelocity + rVecAP.dot(jR * collisionPoint->normal[i]) * bodies[cycle1]->getInvMoI();
								body2AngularVelocityAfter = bodies[cycle2]->angularVelocity + rVecBP.dot(-jR * collisionPoint->normal[i]) * bodies[cycle2]->getInvMoI();

								//Friction impulses
								float mu = (bodies[cycle1]->staticFriction + bodies[cycle2]->staticFriction) / 2.0f;

								spehs::vec2 linearFrictionImpulse;
								if (abs(jLt) < jL * mu)
									linearFrictionImpulse = jLt * tangent;
								else
								{
									linearFrictionImpulse = -jL * tangent * ((bodies[cycle1]->dynamicFriction + bodies[cycle2]->dynamicFriction) / 2.0f);
								}
								float angularFrictionImpulse;
								if (abs(jRt) < jR * mu)
									angularFrictionImpulse = jRt;
								else
								{
									angularFrictionImpulse = -jR * ((bodies[cycle1]->dynamicFriction + bodies[cycle2]->dynamicFriction) / 2.0f);
								}

								body1LinearFrictionAfter = linearFrictionImpulse * bodies[cycle1]->getInvMass();
								body2LinearFrictionAfter = linearFrictionImpulse * bodies[cycle2]->getInvMass();

								body1AngularFrictionAfter = angularFrictionImpulse * bodies[cycle1]->getInvMoI();
								body2AngularFrictionAfter = angularFrictionImpulse * bodies[cycle2]->getInvMoI();

								//Apply impulses
								bodies[cycle1]->applyVelocityImpulse(body1VelocityAfter);
								bodies[cycle2]->applyVelocityImpulse(body2VelocityAfter);
								bodies[cycle1]->applyVelocityImpulse(body1LinearFrictionAfter);
								bodies[cycle2]->applyVelocityImpulse(body2LinearFrictionAfter);

								bodies[cycle1]->applyAngularImpulse(body1AngularVelocityAfter);
								bodies[cycle2]->applyAngularImpulse(body2AngularVelocityAfter);
								bodies[cycle1]->applyAngularImpulse(body1AngularFrictionAfter);
								bodies[cycle2]->applyAngularImpulse(body2AngularFrictionAfter);
							}
							else if (relativeNormalVelocity >= -ZERO_EPSILON && relativeNormalVelocity <= ZERO_EPSILON) //Points are in contact
							{
								//DEbug colors
								if (SHOW_COLORS)
								{
									bodies[cycle1]->ownerObject->getComponent<Sprite>()->setColor(spehs::Color(0,255,0,255));
									bodies[cycle2]->ownerObject->getComponent<Sprite>()->setColor(spehs::Color(0,255,0,255));
								}
								//Positional Correction
								const float percentage = 0.25f;
								const float slop = 0.04f;
								spehs::vec2 correction = std::max(collisionPoint->MTV.getLength() - slop, 0.0f) / (bodies[cycle1]->getInvMass() + bodies[cycle2]->getInvMass()) * percentage * collisionPoint->normal[i];
								if (!bodies[cycle1]->freezePosition && !bodies[cycle1]->isStatic)
									bodies[cycle1]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle1]->ownerObject->getComponent<Transform2D>()->getPosition() + correction * bodies[cycle1]->getInvMass());
								if (!bodies[cycle2]->freezePosition && !bodies[cycle2]->isStatic)
									bodies[cycle2]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle2]->ownerObject->getComponent<Transform2D>()->getPosition() - correction * bodies[cycle2]->getInvMass());
							}
							else //Points are separating
							{
								//DEbug colors
								if (SHOW_COLORS)
								{
									bodies[cycle1]->ownerObject->getComponent<Sprite>()->setColor(spehs::Color(255,255,255,255));
									bodies[cycle2]->ownerObject->getComponent<Sprite>()->setColor(spehs::Color(255,255,255,255));
								}
								//Ignore
								char breakpoint = 0;
							}
						}
					}
				}
			}
		}
	}

	void PhysicsWorld2D::enableGravity(const bool _value)
	{
		useGravity = _value;
	}

	void PhysicsWorld2D::setGravity(const spehs::vec2& _gravity)
	{
		gravity = _gravity;
	}

	float PhysicsWorld2D::j_lin(const float& _e, const spehs::vec2& _velocity, const spehs::vec2& _normal, const float& _invmass1, const float& _invmass2)
	{
		return -(1 + _e) * _velocity.dot(_normal) / _normal.dot(_normal * (_invmass1 + _invmass2));
	}

	float PhysicsWorld2D::j_rot(const float& _e, const spehs::vec2& _velocity, const spehs::vec2& _normal, const float& _invmass1, const float& _invmass2, const spehs::vec2& _rVecAP, const spehs::vec2& _rVecBP, const float& _InvMoIA, const float& _InvMoIB)
	{
		return (-(1 + _e) * _velocity.dot(_normal)) / (_normal.dot(_normal * (_invmass1 + _invmass2)) + (std::pow(_rVecAP.dot(_normal), 2) * _InvMoIA) + (std::pow(_rVecBP.dot(_normal), 2) * _InvMoIB));
	}
}