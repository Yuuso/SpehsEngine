#include "stdafx.h"

#include "SpehsEngine/Physics/SATCollision.h"
#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Math/Geometry.h"

#include "glm/vec2.hpp"
#include "glm/gtx/vector_query.hpp"




namespace se
{
	Projection projectPolygon(const glm::vec2& _axis, Vertex* _vertexArray, const size_t _size)
	{
		float min = glm::dot(_axis, glm::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y));
		float max = min;
		for (size_t i = 0; i < _size; i++)
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

	Projection projectPolygon(const glm::vec2& _axis, glm::vec2* _vertexArray, const size_t _size)
	{
		float min = glm::dot(_axis, glm::vec2(_vertexArray[0].x, _vertexArray[0].y));
		float max = min;
		for (size_t i = 0; i < _size; i++)
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

	glm::vec2 getAxis(Vertex* _vertexArray, const size_t _size, const size_t _i)
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

	glm::vec2 getAxis(glm::vec2* _vertexArray, const size_t _size, const size_t _i)
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

	glm::vec2 getCircleAxis(Vertex* _vertexArray, const size_t _size, const glm::vec2& _circleCenter)
	{
		glm::vec2 polygonVertex = glm::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y);
		for (size_t i = 1; i < _size; i++)
		{
			if (glm::length(glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y) - _circleCenter) < glm::length(polygonVertex - _circleCenter))
			{
				polygonVertex = glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y);
			}
		}
		glm::vec2 edgeVector = polygonVertex - _circleCenter;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};

	glm::vec2 getCircleAxis(glm::vec2* _vertexArray, const size_t _size, const glm::vec2& _circleCenter)
	{
		glm::vec2 polygonVertex = glm::vec2(_vertexArray[0].x, _vertexArray[0].y);
		for (size_t i = 1; i < _size; i++)
		{
			if (glm::length(glm::vec2(_vertexArray[i].x, _vertexArray[i].y) - _circleCenter) < glm::length(polygonVertex - _circleCenter))
			{
				polygonVertex = glm::vec2(_vertexArray[i].x, _vertexArray[i].y);
			}
		}
		glm::vec2 edgeVector = polygonVertex - _circleCenter;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	
#pragma region BOOL COLLISIONS
	bool SATCollision(Vertex* _vertexArray1, const size_t _size1, Vertex* _vertexArray2, const size_t _size2)
	{
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (size_t i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (size_t i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (size_t i = 0; i < _size2; i++)
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
		for (size_t i = 0; i < _size1; i++)
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

	bool SATCollision(glm::vec2* _vertexArray1, const size_t _size1, glm::vec2* _vertexArray2, const size_t _size2)
	{
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (size_t i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (size_t i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (size_t i = 0; i < _size2; i++)
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
		for (size_t i = 0; i < _size1; i++)
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

	bool SATCollision(Vertex* _vertexArray, const size_t _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2 = glm::normalize(getCircleAxis(_vertexArray, _size, _circleCenterPoint));

		//Get all axes
		for (size_t i = 0; i < _size; i++)
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
		for (size_t i = 0; i < _size; i++)
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

	bool SATCollision(glm::vec2* _vertexArray, const size_t _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2 = glm::normalize(getCircleAxis(_vertexArray, _size, _circleCenterPoint));

		//Get all axes
		for (size_t i = 0; i < _size; i++)
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
		for (size_t i = 0; i < _size; i++)
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
		if (glm::length(_circleCenterPoint1 - _circleCenterPoint2) < (_circleRadius1 + _circleRadius2))
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
			if (glm::length(_rayPosition - _circleCenterPoint) <= _rayDistance)
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
			return glm::length(_circleCenterPoint - _rayPosition); //TEMPs
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
		vec /= (float)_vertexArrayLength;

		return magnitude(vec - _rayCastPosition);
	}
#pragma endregion

	
#pragma region COLLISIONPOINT COLLISIONS
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray1, const size_t _size1, Vertex* _vertexArray2, const size_t _size2)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (size_t i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (size_t i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (size_t i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return false;
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
		for (size_t i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return false;
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
		deposit.MTV = smallestAxis * abs(overlap);
		
		//Recalculate all axes without normalization
		for (size_t i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (size_t i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}

		glm::vec2 testPoint;
		for (size_t i = 0; i < _size1; i++)
		{
			testPoint = glm::vec2(_vertexArray1[i].position.x, _vertexArray1[i].position.y);
			//Find which points are colliding
			if (SATCollision(_vertexArray2, _size2, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis2[0]));

				//Find normal
				glm::vec2 pointVector = testPoint - getCenter(_vertexArray2, _size2);
				for (unsigned i = 1; i < _size2; i++)
				{
					if (glm::dot(pointVector, axis2[i]) > glm::dot(pointVector, deposit.points.back().normal))
					{
						deposit.points.back().normal = axis2[i];
					}
				}
			}
		}

		for (size_t i = 0; i < _size2; i++)
		{
			testPoint = glm::vec2(_vertexArray2[i].position.x, _vertexArray2[i].position.y);
			//Find which points are colliding
			if (SATCollision(_vertexArray1, _size1, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis1[0]));

				//Find normal
				glm::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray1, _size1);
				for (unsigned i = 1; i < _size1; i++)
				{
					if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, deposit.points.back().normal))
					{
						deposit.points.back().normal = axis1[i];
					}
				}
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		for (size_t i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal = glm::normalize(deposit.points[i].normal);
		}

		delete [] axis1;
		delete [] axis2;
		return true;
	}

	bool SATMTVCollision(CollisionResults& deposit, glm::vec2* _vertexArray1, const size_t _size1, glm::vec2* _vertexArray2, const size_t _size2)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (size_t i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(getAxis(_vertexArray1, _size1, i));
		}
		for (size_t i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(getAxis(_vertexArray2, _size2, i));
		}

		//Loop through axes2
		for (size_t i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return false;
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
		for (size_t i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete [] axis1;
				delete [] axis2;
				return false;
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
		deposit.MTV = smallestAxis * abs(overlap);

		//Recalculate all axes without normalization
		for (size_t i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (size_t i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}

		glm::vec2 testPoint;
		for (size_t i = 0; i < _size1; i++)
		{
			testPoint = glm::vec2(_vertexArray1[i].x, _vertexArray1[i].y);
			//Find which points are colliding
			if (SATCollision(_vertexArray2, _size2, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis2[0]));

				//Find normal
				glm::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray2, _size2);
				for (unsigned i = 1; i < _size2; i++)
				{
					if (glm::dot(pointVector, axis2[i]) > glm::dot(pointVector, deposit.points.back().normal))
					{
						deposit.points.back().normal = axis2[i];
					}
				}
			}
		}

		for (size_t i = 0; i < _size2; i++)
		{
			testPoint = glm::vec2(_vertexArray2[i].x, _vertexArray2[i].y);
			//Find which points are colliding
			if (SATCollision(_vertexArray1, _size1, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis1[0]));

				//Find normal
				glm::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray1, _size1);
				for (unsigned i = 1; i < _size1; i++)
				{
					if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, deposit.points.back().normal))
					{
						deposit.points.back().normal = axis1[i];
					}
				}
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		for (size_t i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal = glm::normalize(deposit.points[i].normal);
		}

		delete [] axis1;
		delete [] axis2;
		return true;
	}

	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray, const size_t _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (size_t i = 0; i < _size; i++)
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
			return false;
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
		for (size_t i = 0; i < _size; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray, _size);
			Projection p2 = projectCircle(axis1[i], _circleCenterPoint, _circleRadius);
			if (!p1.overlap(p2))
			{
				delete axis1;
				return false;
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
		deposit.MTV = glm::normalize(smallestAxis) * abs(overlap);
		
		//Recalculate all axes without normalizing
		for (size_t i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);


		glm::vec2 testPoint;
		for (size_t i = 0; i < _size; i++)
		{
			testPoint = glm::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y);
			//Find which points are colliding
			if (circleCollision(_circleCenterPoint, _circleRadius, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, testPoint - _circleCenterPoint));

				//Find normal
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		glm::vec2 circlePoint = glm::normalize(getCenter(_vertexArray, _size) - _circleCenterPoint);
		circlePoint = _circleCenterPoint + circlePoint * _circleRadius;
		//Find which points are colliding
		if (SATCollision(_vertexArray, _size, circlePoint, 0.0f))
		{
			deposit.points.push_back(CollisionResults::Point(circlePoint, axis1[0]));

			//Find normal
			glm::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray, _size);
			for (unsigned i = 1; i < _size; i++)
			{
				if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, deposit.points.back().normal))
				{
					deposit.points.back().normal = axis1[i];
				}
			}
		}

		//Normalize normals
		for (size_t i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal = glm::normalize(deposit.points[i].normal);
		}

		delete [] axis1;
		return true;
	}

	bool SATMTVCollision(CollisionResults& deposit, glm::vec2* _vertexArray, const size_t _size, const glm::vec2& _circleCenterPoint, const float _circleRadius)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (size_t i = 0; i < _size; i++)
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
			return false;
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
		for (size_t i = 0; i < _size; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray, _size);
			Projection p2 = projectCircle(axis1[i], _circleCenterPoint, _circleRadius);
			if (!p1.overlap(p2))
			{
				delete axis1;
				return false;
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
		deposit.MTV = glm::normalize(smallestAxis) * abs(overlap);

		//Recalculate all axes without normalizing
		for (size_t i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);


		glm::vec2 testPoint;
		for (size_t i = 0; i < _size; i++)
		{
			testPoint = glm::vec2(_vertexArray[i].x, _vertexArray[i].y);
			//Find which points are colliding
			if (circleCollision(_circleCenterPoint, _circleRadius, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, testPoint - _circleCenterPoint));

				//Find normal
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		glm::vec2 circlePoint = glm::normalize(getCenter(_vertexArray, _size) - _circleCenterPoint);
		circlePoint = _circleCenterPoint + circlePoint * _circleRadius;
		//Find which points are colliding
		if (SATCollision(_vertexArray, _size, circlePoint, 0.0f))
		{
			deposit.points.push_back(CollisionResults::Point(circlePoint, axis1[0]));

			//Find normal
			glm::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray, _size);
			for (unsigned i = 1; i < _size; i++)
			{
				if (glm::dot(pointVector, axis1[i]) > glm::dot(pointVector, deposit.points.back().normal))
				{
					deposit.points.back().normal = axis1[i];
				}
			}
		}

		//Normalize normals
		for (size_t i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal = glm::normalize(deposit.points[i].normal);
		}

		delete [] axis1;
		return false;
	}

	bool circleMTVCollision(CollisionResults& deposit, const glm::vec2& _circleCenterPoint1, const float _circleRadius1, const glm::vec2& _circleCenterPoint2, const float _circleRadius2)
	{
		if (glm::length(_circleCenterPoint1 - _circleCenterPoint2) < (_circleRadius1 + _circleRadius2))
		{
			if (_circleCenterPoint1 == _circleCenterPoint2) //Can't normalize 0 vectors
			{
				deposit.MTV = glm::vec2(1.0f, 0.0f) * (glm::length(_circleCenterPoint1 - _circleCenterPoint2) - (_circleRadius1 + _circleRadius2));
				deposit.points.push_back(CollisionResults::Point(_circleCenterPoint1 + glm::vec2(1.0f, 0.0f) * _circleRadius1, deposit.MTV));
			}
			else
			{
				deposit.MTV = glm::normalize(_circleCenterPoint2 - _circleCenterPoint1) * (glm::length(_circleCenterPoint1 - _circleCenterPoint2) - (_circleRadius1 + _circleRadius2));
				deposit.points.push_back(CollisionResults::Point(_circleCenterPoint1 + glm::normalize(_circleCenterPoint2 - _circleCenterPoint1) * _circleRadius1, deposit.MTV));
			}
			return true;
		}
		return false;
	}
#pragma endregion
}