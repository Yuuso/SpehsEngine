
#include "RigidBody2D.h"
#include "PhysicsWorld2D.h"
#include "SATCollision.h"
#include "GameObject.h"
#include "Transform2D.h"

#include "Sprite.h"
#include "Polygon.h"

#include <glm/gtx/vector_query.hpp>

#define ZERO_EPSILON 0.000001f

#define SHOW_COLORS false


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
			if (!glm::isNull(gravity, ZERO_EPSILON))
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
						for (unsigned i = 0; i < collisionPoint->point.size(); i++)
						{
							glm::vec2 body1VelocityBefore = bodies[cycle1]->getVelocityAtPosition(collisionPoint->point[i]);
							glm::vec2 body2VelocityBefore = bodies[cycle2]->getVelocityAtPosition(collisionPoint->point[i]);
							float relativeNormalVelocity = glm::dot((body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i]);

							//Separate
							//float body1Percentage = glm::length(body1VelocityBefore) / (glm::length(body1VelocityBefore) + glm::length(body2VelocityBefore));
							//if (body1Percentage != body1Percentage) //if 2 zero-velocity bodies collide
							//	body1Percentage = 0.5f;
							//if (glm::dot(body1VelocityBefore, collisionPoint->MTV) > 0.0f)
							//{
							//	bodies[cycle1]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle1]->ownerObject->getComponent<Transform2D>()->getPosition() - collisionPoint->MTV * body1Percentage);
							//	bodies[cycle2]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle2]->ownerObject->getComponent<Transform2D>()->getPosition() + collisionPoint->MTV * (1.0f - body1Percentage));
							//}
							//else
							//{
							//	bodies[cycle1]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle1]->ownerObject->getComponent<Transform2D>()->getPosition() + collisionPoint->MTV * body1Percentage);
							//	bodies[cycle2]->ownerObject->getComponent<Transform2D>()->setPosition(bodies[cycle2]->ownerObject->getComponent<Transform2D>()->getPosition() - collisionPoint->MTV * (1.0f - body1Percentage));
							//}

							//Resolve collisions
							if (relativeNormalVelocity < -ZERO_EPSILON) //Point are colliding
							{
								//DEbug colors
								if (SHOW_COLORS)
								{
									bodies[cycle1]->ownerObject->getComponent<Sprite>()->sprite->setColor(spehs::RED);
									bodies[cycle2]->ownerObject->getComponent<Sprite>()->sprite->setColor(spehs::RED);
								}

								glm::vec2 body1VelocityAfter;
								glm::vec2 body2VelocityAfter;
								float body1AngularVelocityAfter;
								float body2AngularVelocityAfter;
								float e = (bodies[cycle1]->elasticity + bodies[cycle2]->elasticity) / 2.0f;
								//if (abs(body1VelocityBefore - body2VelocityBefore).x < ZERO_EPSILON || abs(body1VelocityBefore - body2VelocityBefore).x < ZERO_EPSILON)
								//	e = 0.0f;

								glm::vec2 rVecAP = glm::vec2(-(collisionPoint->point[i] - bodies[cycle1]->centerOfMass).y, (collisionPoint->point[i] - bodies[cycle1]->centerOfMass).x);
								glm::vec2 rVecBP = glm::vec2(-(collisionPoint->point[i] - bodies[cycle2]->centerOfMass).y, (collisionPoint->point[i] - bodies[cycle2]->centerOfMass).x);

								if (bodies[cycle1]->isStatic)
								{
									if (bodies[cycle2]->isStatic)
									{
										body1VelocityAfter = body1VelocityBefore;
										body2VelocityAfter = body2VelocityBefore;

										body1AngularVelocityAfter = bodies[cycle1]->angularVelocity;
										body2AngularVelocityAfter = bodies[cycle2]->angularVelocity;
									}
									else
									{
										body1VelocityAfter = body1VelocityBefore;

										body2VelocityAfter = body2VelocityBefore + (
											-j_lin(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], 0.0f, bodies[cycle2]->mass, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia)
											/ bodies[cycle2]->mass) * collisionPoint->normal[i];

										body1AngularVelocityAfter = bodies[cycle1]->angularVelocity;

										body2AngularVelocityAfter = bodies[cycle2]->angularVelocity + glm::dot(rVecBP,
											-j_rot(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], 0.0f, bodies[cycle2]->mass, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia) * collisionPoint->normal[i])
											/ bodies[cycle2]->momentOfInertia;
									}
								}
								else if (bodies[cycle2]->isStatic)
								{
									body1VelocityAfter = body1VelocityBefore + (
										j_lin(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], bodies[cycle1]->mass, 0.0f, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia)
										/ bodies[cycle1]->mass) * collisionPoint->normal[i];

									body2VelocityAfter = body2VelocityBefore;

									body1AngularVelocityAfter = bodies[cycle1]->angularVelocity + glm::dot(rVecAP,
										j_rot(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], bodies[cycle1]->mass, 0.0f, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia) * collisionPoint->normal[i])
										/ bodies[cycle1]->momentOfInertia;

									body2AngularVelocityAfter = bodies[cycle2]->angularVelocity;
								}
								else
								{
									body1VelocityAfter = body1VelocityBefore + (
										j_lin(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], bodies[cycle1]->mass, bodies[cycle2]->mass, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia)
										/ bodies[cycle1]->mass) * collisionPoint->normal[i];

									body2VelocityAfter = body2VelocityBefore + (
										-j_lin(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], bodies[cycle1]->mass, bodies[cycle2]->mass, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia)
										/ bodies[cycle2]->mass) * collisionPoint->normal[i];

									body1AngularVelocityAfter = bodies[cycle1]->angularVelocity + glm::dot(rVecAP,
										j_rot(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], bodies[cycle1]->mass, bodies[cycle2]->mass, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia) * collisionPoint->normal[i])
										/ bodies[cycle1]->momentOfInertia;

									body2AngularVelocityAfter = bodies[cycle2]->angularVelocity + glm::dot(rVecBP,
										-j_rot(e, (body1VelocityBefore - body2VelocityBefore), collisionPoint->normal[i], bodies[cycle1]->mass, bodies[cycle2]->mass, rVecAP, rVecBP, bodies[cycle1]->momentOfInertia, bodies[cycle2]->momentOfInertia) * collisionPoint->normal[i])
										/ bodies[cycle2]->momentOfInertia;
								}

								bodies[cycle1]->applyVelocityImpulse(body1VelocityAfter / (float)collisionPoint->point.size());
								bodies[cycle2]->applyVelocityImpulse(body2VelocityAfter / (float) collisionPoint->point.size());
								bodies[cycle1]->applyAngularImpulse(body1AngularVelocityAfter / (float) collisionPoint->point.size());
								bodies[cycle2]->applyAngularImpulse(body2AngularVelocityAfter / (float) collisionPoint->point.size());
							}
							else if (relativeNormalVelocity >= -ZERO_EPSILON && relativeNormalVelocity <= ZERO_EPSILON) //Points are in contact
							{
								//DEbug colors
								if (SHOW_COLORS)
								{
									bodies[cycle1]->ownerObject->getComponent<Sprite>()->sprite->setColor(spehs::GREEN);
									bodies[cycle2]->ownerObject->getComponent<Sprite>()->sprite->setColor(spehs::GREEN);
								}

								bodies[cycle1]->applySupportForce(collisionPoint->normal[i], collisionPoint->point[i]);
								bodies[cycle2]->applySupportForce(collisionPoint->normal[i], collisionPoint->point[i]);
							}
							else //Points are separating
							{
								//DEbug colors
								if (SHOW_COLORS)
								{
									bodies[cycle1]->ownerObject->getComponent<Sprite>()->sprite->setColor(spehs::WHITE);
									bodies[cycle2]->ownerObject->getComponent<Sprite>()->sprite->setColor(spehs::WHITE);
								}
								//Ignore
								char breakpoint = 0;
							}
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

	float PhysicsWorld2D::j_lin(const float& _e, const glm::vec2& _velocity, const glm::vec2& _normal, const float& _mass1, const float& _mass2, const glm::vec2& _rVecAP, const glm::vec2& _rVecBP, const float& _MoIA, const float& _MoIB)
	{
		if (_mass1 == 0.0f) //if 1 is static
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / (glm::dot(_normal, _normal*(1 / _mass2)));
		}
		else if (_mass2 == 0.0f) //if 2 is static
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / (glm::dot(_normal, _normal*(1 / _mass1)));
		}
		else
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / (glm::dot(_normal, _normal*((1 / _mass1) + (1 / _mass2))));
		}
	}

	float PhysicsWorld2D::j_rot(const float& _e, const glm::vec2& _velocity, const glm::vec2& _normal, const float& _mass1, const float& _mass2, const glm::vec2& _rVecAP, const glm::vec2& _rVecBP, const float& _MoIA, const float& _MoIB)
	{
		if (_mass1 == 0.0f) //if 1 is static
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / (glm::dot(_normal, _normal*(1 / _mass2)) + (pow(glm::dot(_rVecBP, _normal), 2) / _MoIB) );
		}
		else if (_mass2 == 0.0f) //if 2 is static
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / (glm::dot(_normal, _normal*(1 / _mass1)) + (pow(glm::dot(_rVecAP, _normal), 2) / _MoIA) );
		}
		else
		{
			return glm::dot(-(1 + _e)*_velocity, _normal) / (glm::dot(_normal, _normal*((1 / _mass1) + (1 / _mass2))) + (pow(glm::dot(_rVecAP, _normal), 2) / _MoIA) + (pow(glm::dot(_rVecBP, _normal), 2) / _MoIB) );
		}
	}
}