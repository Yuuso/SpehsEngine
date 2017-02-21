
#include "Console.h"
#include "SATCollision.h"
#include "Vertex.h"
#include "Geometry.h"

#include "glm/vec2.hpp"
#include "glm/gtx/vector_query.hpp"

#include <atomic>


std::atomic<int> collisionPointAllocations;
std::atomic<int> collisionPointDeallocations;



namespace spehs
{
	CollisionPoint::CollisionPoint()
	{
#ifdef _DEBUG
		collisionPointAllocations++;
#endif
	}
	CollisionPoint::~CollisionPoint()
	{
#ifdef _DEBUG
		collisionPointDeallocations++;
#endif
	}

	Projection projectPolygon(const glm::vec2& _axis, Vertex* _vertexArray, const unsigned int _size)
	{
		float min = glm::dot(_axis, glm::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y));
		float max = min;
		for (int i = 0; i < _size; i++)
		{
			float x = glm::dot(_axis, glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y));
			if (x < min)
				min = x;
			else if (x > max)
				max = x;
		}
		Projection returnProj(min, max);
		return returnProj;
	};
	Projection projectPolygon(const glm::vec2& _axis, glm::vec2* _vertexArray, const unsigned int _size)
	{
		float min = glm::dot(_axis, glm::vec2(_vertexArray[0].x, _vertexArray[0].y));
		float max = min;
		for (int i = 0; i < _size; i++)
		{
			float x = glm::dot(_axis, glm::vec2(_vertexArray[i].x, _vertexArray[i].y));
			if (x < min)
				min = x;
			else if (x > max)
				max = x;
		}
		Projection returnProj(min, max);
		return returnProj;
	};
	Projection projectCircle(const glm::vec2& _axis, const glm::vec2& _vertex, const float _radius)
	{
		float min = glm::dot(_axis, glm::vec2(_vertex.x, _vertex.y));
		float max = min + _radius;
		min -= _radius;
		Projection returnProj(min, max);
		return returnProj;
	};

	glm::vec2 getAxis(Vertex* _vertexArray, const unsigned int _size, const unsigned int _i)
	{
		glm::vec2 currentVertex(_vertexArray[_i].position.x, _vertexArray[_i].position.y);
		glm::vec2 nextVertex;
		if (_i + 1 != _size)
			nextVertex = glm::vec2(_vertexArray[_i + 1].position.x, _vertexArray[_i + 1].position.y);
		else
			nextVertex = glm::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y);

		glm::vec2 edgeVector = currentVertex - nextVertex;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	glm::vec2 getAxis(glm::vec2* _vertexArray, const unsigned int _size, const unsigned int _i)
	{
		glm::vec2 currentVertex = glm::vec2(_vertexArray[_i].x, _vertexArray[_i].y);
		glm::vec2 nextVertex;
		if (_i + 1 != _size)
			nextVertex = glm::vec2(_vertexArray[_i + 1].x, _vertexArray[_i + 1].y);
		else
			nextVertex = glm::vec2(_vertexArray[0].x, _vertexArray[0].y);

		glm::vec2 edgeVector = currentVertex - nextVertex;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	glm::vec2 getCircleAxis(Vertex* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenter)
	{
		glm::vec2 polygonVertex = glm::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y);
		for (int i = 1; i < _size; i++)
		{
			if (glm::distance(glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y), _circleCenter) < glm::distance(polygonVertex, _circleCenter))
			{
				polygonVertex = glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y);
			}
		}
		glm::vec2 edgeVector = polygonVertex - _circleCenter;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	glm::vec2 getCircleAxis(glm::vec2* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenter)
	{
		glm::vec2 polygonVertex = glm::vec2(_vertexArray[0].x, _vertexArray[0].y);
		for (int i = 1; i < _size; i++)
		{
			if (glm::distance(glm::vec2(_vertexArray[i].x, _vertexArray[i].y), _circleCenter) < glm::distance(polygonVertex, _circleCenter))
			{
				polygonVertex = glm::vec2(_vertexArray[i].x, _vertexArray[i].y);
			}
		}
		glm::vec2 edgeVector = polygonVertex - _circleCenter;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	
#pragma region BOOL COLLISIONS
	bool SATCollision(Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2)
	{
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (unsigned int i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return false;
			}
		}

		//Loop through axes1
		for (unsigned int i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return false;
			}
		}

		//Here we know that no separating axis was found and there is a collision
		delete [] axis1;
		delete [] axis2;
		return true;
	}
	bool SATCollision(glm::vec2* _vertexArray1, const unsigned int _size1, glm::vec2* _vertexArray2, const unsigned int _size2)
	{
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (unsigned int i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return false;
			}
		}

		//Loop through axes1
		for (unsigned int i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return false;
			}
		}

		//Here we know that no separating axis was found and there is a collision
		delete [] axis1;
		delete [] axis2;
		return true;
	}
	bool SATCollision(Vertex* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2 = glm::normalize(getCircleAxis(_vertexArray, _size, _circleCenterPoint));

		//Get all axes
		for (unsigned int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray, _size, i));
		}

		//Go through axis2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return false;
		}

		//Loop through axes1
		for (unsigned int i = 0; i < _size; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray, _size);
			Projection p2 = projectCircle(axis1[i], _circleCenterPoint, _circleRadius);
			if (!p1.overlap(p2))
			{
				delete axis1;
				return false;
			}
		}

		//Here we know that no separating axis was found and there is a collision
		delete [] axis1;
		return true;
	}
	bool SATCollision(glm::vec2* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2 = glm::normalize(getCircleAxis(_vertexArray, _size, _circleCenterPoint));

		//Get all axes
		for (unsigned int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray, _size, i));
		}

		//Go through axis2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return false;
		}

		//Loop through axes1
		for (unsigned int i = 0; i < _size; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray, _size);
			Projection p2 = projectCircle(axis1[i], _circleCenterPoint, _circleRadius);
			if (!p1.overlap(p2))
			{
				delete axis1;
				return false;
			}
		}

		//Here we know that no separating axis was found and there is a collision
		delete [] axis1;
		return true;
	}
	bool circleCollision(const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2)
	{
		if (glm::distance(_circleCenterPoint1, _circleCenterPoint2) < (_circleRadius1 + _circleRadius2))
		{
			return true;
		}
		return false;
	}

	bool rayCollision(const glm::vec2 _rayPosition, const float _rayDirection, const glm::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance)
	{
		glm::vec2 endPoint(_rayPosition.x + _rayDistance * cos(_rayDirection), _rayPosition.y + _rayDistance * sin(_rayDirection));
		if (glm::dot(endPoint - _rayPosition, _circleCenterPoint - _rayPosition) < 0.0f)
		{
			return false;
		}
		if (pointToLineDistance(_rayPosition, endPoint, _circleCenterPoint) <= _circleRadius)
		{
			if (glm::distance(_rayPosition, _circleCenterPoint) <= _rayDistance)
			{
				return true;
			}
		}
		return false;
	}
	float rayCollisionDistance(const glm::vec2 _rayPosition, const float _rayDirection, const glm::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance)
	{
		glm::vec2 endPoint(_rayPosition.x + _rayDistance * cos(_rayDirection), _rayPosition.y + _rayDistance * sin(_rayDirection));
		float distance = pointToLineDistance(_rayPosition, endPoint, _circleCenterPoint);
		if (distance <= _circleRadius)
		{
			return glm::distance(_circleCenterPoint, _rayPosition); //TEMPs
		}
		return 0.0f;
	}

	bool rayCollision(const glm::vec2 _rayCastPosition, float _rayDirection, glm::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance)
	{
		return false;
	}
	float rayCollisionDistance(const glm::vec2 _rayCastPosition, float _rayDirection, glm::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance)
	{
		glm::vec2 vec(0.0f, 0.0f);
		for (unsigned i = 0; i < _vertexArrayLength; i++)
		{
			vec.x += _vertexArray[i].x;
			vec.y += _vertexArray[i].y;
		}
		vec /= _vertexArrayLength;

		return magnitude(vec - _rayCastPosition);
	}
#pragma endregion

	
#pragma region COLLISIONPOINT COLLISIONS
	std::shared_ptr<CollisionPoint> SATMTVCollision(Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (int i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return nullptr;
			}
			else
			{
				float o = p1.getOverlap(p2);
				if (o < overlap)
				{
					overlap = o;
					smallestAxis = axis2[i];
				}
			}
		}

		//Loop through axes1
		for (unsigned int i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return nullptr;
			}
			else
			{
				float o = p1.getOverlap(p2);
				if (o < overlap)
				{
					overlap = o;
					smallestAxis = axis1[i];
				}
			}
		}

		//Here we know that no separating axis was found and there is a collision
		//Calculate collision point
		std::shared_ptr<CollisionPoint> result(new CollisionPoint);
		result->MTV = smallestAxis * abs(overlap);
		
		//Recalculate all axes without normalization
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}

		glm::vec2 testPoint;
		for (unsigned int i = 0; i < _size1; i++)
		{
			testPoint = glm::vec2(_vertexArray1[i].position.x, _vertexArray1[i].position.y);
			//Find which points are colliding
			if (SATCollision(_vertexArray2, _size2, testPoint, 0.0f))
			{
				result->point.push_back(testPoint);

				//Find normal
				glm::vec2 pointVector = result->point.back() - getCenter(_vertexArray2, _size2);
				result->normal.push_back(axis2[0]);
				for (unsigned i = 1; i < _size2; i++)
				{
					if (glm::dot(pointVector, axis2[i]) > glm::dot(pointVector, result->normal.back()))
					{
						result->normal.back() = axis2[i];
					}
				}
			}
		}

		for (unsigned int i = 0; i < _size2; i++)
		{
			testPoint = glm::vec2(_vertexArray2[i].position.x, _vertexArray2[i].position.y);
			//Find which points are colliding
			if (SATCollision(_vertexArray1, _size1, testPoint, 0.0f))
			{
				result->point.push_back(testPoint);

				//Find normal
				glm::vec2 pointVector = result->point.back() - getCenter(_vertexArray1, _size1);
				result->normal.push_back(axis1[0]);
				for (unsigned i = 1; i < _size1; i++)
				{
					if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, result->normal.back()))
					{
						result->normal.back() = axis1[i];
					}
				}
				result->normal.back() = -result->normal.back(); //?
			}
		}

		for (unsigned int i = 0; i < result->normal.size(); i++)
		{
			result->normal[i] = glm::normalize(result->normal[i]);
		}

		delete [] axis1;
		delete [] axis2;
		return result;
	}
	std::shared_ptr<CollisionPoint> SATMTVCollision(glm::vec2* _vertexArray1, const unsigned int _size1, glm::vec2* _vertexArray2, const unsigned int _size2)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (int i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return nullptr;
			}
			else
			{
				float o = p1.getOverlap(p2);
				if (o < overlap)
				{
					overlap = o;
					smallestAxis = axis2[i];
				}
			}
		}

		//Loop through axes1
		for (unsigned int i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return nullptr;
			}
			else
			{
				float o = p1.getOverlap(p2);
				if (o < overlap)
				{
					overlap = o;
					smallestAxis = axis1[i];
				}
			}
		}

		//Here we know that no separating axis was found and there is a collision
		//Calculate collision point
		std::shared_ptr<CollisionPoint> result(new CollisionPoint);
		result->MTV = smallestAxis * abs(overlap);

		//Recalculate all axes without normalization
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}

		glm::vec2 testPoint;
		for (unsigned int i = 0; i < _size1; i++)
		{
			testPoint = glm::vec2(_vertexArray1[i].x, _vertexArray1[i].y);
			//Find which points are colliding
			if (SATCollision(_vertexArray2, _size2, testPoint, 0.0f))
			{
				result->point.push_back(testPoint);

				//Find normal
				glm::vec2 pointVector = result->point.back() - getCenter(_vertexArray2, _size2);
				result->normal.push_back(axis2[0]);
				for (unsigned i = 1; i < _size2; i++)
				{
					if (glm::dot(pointVector, axis2[i]) > glm::dot(pointVector, result->normal.back()))
					{
						result->normal.back() = axis2[i];
					}
				}
			}
		}

		for (unsigned int i = 0; i < _size2; i++)
		{
			testPoint = glm::vec2(_vertexArray2[i].x, _vertexArray2[i].y);
			//Find which points are colliding
			if (SATCollision(_vertexArray1, _size1, testPoint, 0.0f))
			{
				result->point.push_back(testPoint);

				//Find normal
				glm::vec2 pointVector = result->point.back() - getCenter(_vertexArray1, _size1);
				result->normal.push_back(axis1[0]);
				for (unsigned i = 1; i < _size1; i++)
				{
					if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, result->normal.back()))
					{
						result->normal.back() = axis1[i];
					}
				}
				result->normal.back() = -result->normal.back(); //?
			}
		}

		for (unsigned int i = 0; i < result->normal.size(); i++)
		{
			result->normal[i] = glm::normalize(result->normal[i]);
		}

		delete [] axis1;
		delete [] axis2;
		return result;
	}
	std::shared_ptr<CollisionPoint> SATMTVCollision(Vertex* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray, _size, i));
		}
		axis2 = glm::normalize(getCircleAxis(_vertexArray, _size, _circleCenterPoint));

		//Go through axes2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return nullptr;
		}
		else
		{
			float o = p1.getOverlap(p2);
			if (o < overlap)
			{
				overlap = o;
				smallestAxis = axis2;
			}
		}

		//Loop through axes1
		for (int i = 0; i < _size; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray, _size);
			Projection p2 = projectCircle(axis1[i], _circleCenterPoint, _circleRadius);
			if (!p1.overlap(p2))
			{
				delete axis1;
				return nullptr;
			}
			else
			{
				float o = p1.getOverlap(p2);
				if (o < overlap)
				{
					overlap = o;
					smallestAxis = axis1[i];
				}
			}
		}

		//Here we know that no separating axis was found and there is a collision
		//Calculate collision point
		std::shared_ptr<CollisionPoint> result(new CollisionPoint);
		result->MTV = glm::normalize(smallestAxis) * abs(overlap);
		
		//Recalculate all axes without normalizing
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);


		glm::vec2 testPoint;
		for (unsigned int i = 0; i < _size; i++)
		{
			testPoint = glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y);
			//Find which points are colliding
			if (circleCollision(_circleCenterPoint, _circleRadius, testPoint, 0.0f))
			{
				result->point.push_back(testPoint);

				//Find normal
				result->normal.push_back(glm::vec2(testPoint - _circleCenterPoint));
				result->normal.back() = -result->normal.back(); //?
			}
		}

		glm::vec2 circlePoint = glm::normalize(getCenter(_vertexArray, _size) - _circleCenterPoint);
		circlePoint = _circleCenterPoint + circlePoint * _circleRadius;
		//Find which points are colliding
		if (SATCollision(_vertexArray, _size, circlePoint, 0.0f))
		{
			result->point.push_back(circlePoint);

			//Find normal
			glm::vec2 pointVector = result->point.back() - getCenter(_vertexArray, _size);
			result->normal.push_back(axis1[0]);
			for (unsigned i = 1; i < _size; i++)
			{
				if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, result->normal.back()))
				{
					result->normal.back() = axis1[i];
				}
			}
		}

		//Normalize normals
		for (unsigned int i = 0; i < result->normal.size(); i++)
		{
			result->normal[i] = glm::normalize(result->normal[i]);
		}

		delete [] axis1;
		return result;
	}
	std::shared_ptr<CollisionPoint> SATMTVCollision(glm::vec2* _vertexArray, const unsigned int _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray, _size, i));
		}
		axis2 = glm::normalize(getCircleAxis(_vertexArray, _size, _circleCenterPoint));

		//Go through axes2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return nullptr;
		}
		else
		{
			float o = p1.getOverlap(p2);
			if (o < overlap)
			{
				overlap = o;
				smallestAxis = axis2;
			}
		}

		//Loop through axes1
		for (int i = 0; i < _size; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray, _size);
			Projection p2 = projectCircle(axis1[i], _circleCenterPoint, _circleRadius);
			if (!p1.overlap(p2))
			{
				delete axis1;
				return nullptr;
			}
			else
			{
				float o = p1.getOverlap(p2);
				if (o < overlap)
				{
					overlap = o;
					smallestAxis = axis1[i];
				}
			}
		}

		//Here we know that no separating axis was found and there is a collision
		//Calculate collision point
		std::shared_ptr<CollisionPoint> result(new CollisionPoint);
		result->MTV = glm::normalize(smallestAxis) * abs(overlap);

		//Recalculate all axes without normalizing
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);


		glm::vec2 testPoint;
		for (unsigned int i = 0; i < _size; i++)
		{
			testPoint = glm::vec2(_vertexArray[i].x, _vertexArray[i].y);
			//Find which points are colliding
			if (circleCollision(_circleCenterPoint, _circleRadius, testPoint, 0.0f))
			{
				result->point.push_back(testPoint);

				//Find normal
				result->normal.push_back(glm::vec2(testPoint - _circleCenterPoint));
				result->normal.back() = -result->normal.back(); //?
			}
		}

		glm::vec2 circlePoint = glm::normalize(getCenter(_vertexArray, _size) - _circleCenterPoint);
		circlePoint = _circleCenterPoint + circlePoint * _circleRadius;
		//Find which points are colliding
		if (SATCollision(_vertexArray, _size, circlePoint, 0.0f))
		{
			result->point.push_back(circlePoint);

			//Find normal
			glm::vec2 pointVector = result->point.back() - getCenter(_vertexArray, _size);
			result->normal.push_back(axis1[0]);
			for (unsigned i = 1; i < _size; i++)
			{
				if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, result->normal.back()))
				{
					result->normal.back() = axis1[i];
				}
			}
		}

		//Normalize normals
		for (unsigned int i = 0; i < result->normal.size(); i++)
		{
			result->normal[i] = glm::normalize(result->normal[i]);
		}

		delete [] axis1;
		return result;
	}
	std::shared_ptr<CollisionPoint> circleMTVCollision(const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2)
	{
		if (glm::distance(_circleCenterPoint1, _circleCenterPoint2) < (_circleRadius1 + _circleRadius2))
		{
			std::shared_ptr<CollisionPoint> result(new CollisionPoint);
			if (_circleCenterPoint1 == _circleCenterPoint2) //Can't normalize 0 vectors
			{
				result->MTV = glm::vec2(1.0f, 0.0f) *(glm::distance(_circleCenterPoint1, _circleCenterPoint2) - (_circleRadius1 + _circleRadius2));
				result->point.push_back(_circleCenterPoint1 + glm::vec2(1.0f, 0.0f) * _circleRadius1);
				result->normal.push_back(result->MTV);
			}
			else
			{
				result->MTV = glm::normalize(_circleCenterPoint2 - _circleCenterPoint1) * (glm::distance(_circleCenterPoint1, _circleCenterPoint2) - (_circleRadius1 + _circleRadius2));
				result->point.push_back(_circleCenterPoint1 + glm::normalize(_circleCenterPoint2 - _circleCenterPoint1) * _circleRadius1);
				result->normal.push_back(result->MTV);
			}
			return result;
		}
		return nullptr;
	}
#pragma endregion
}