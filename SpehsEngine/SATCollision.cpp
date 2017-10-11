
#include "Console.h"
#include "SATCollision.h"
#include "Vertex.h"
#include "Geometry.h"

#include "glm/vec2.hpp"
#include "glm/gtx/vector_query.hpp"




namespace spehs
{
	Projection projectPolygon(const spehs::vec2& _axis, Vertex* _vertexArray, const unsigned int _size)
	{
		float min = _axis.dot(spehs::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y));
		float max = min;
		for (int i = 0; i < _size; i++)
		{
			float x = _axis.dot(spehs::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y));
			if (x < min)
				min = x;
			else if (x > max)
				max = x;
		}
		Projection returnProj(min, max);
		return returnProj;
	};
	Projection projectPolygon(const spehs::vec2& _axis, spehs::vec2* _vertexArray, const unsigned int _size)
	{
		float min = _axis.dot(spehs::vec2(_vertexArray[0].x, _vertexArray[0].y));
		float max = min;
		for (int i = 0; i < _size; i++)
		{
			float x = _axis.dot(spehs::vec2(_vertexArray[i].x, _vertexArray[i].y));
			if (x < min)
				min = x;
			else if (x > max)
				max = x;
		}
		Projection returnProj(min, max);
		return returnProj;
	};
	Projection projectCircle(const spehs::vec2& _axis, const spehs::vec2& _vertex, const float _radius)
	{
		float min = _axis.dot(spehs::vec2(_vertex.x, _vertex.y));
		float max = min + _radius;
		min -= _radius;
		Projection returnProj(min, max);
		return returnProj;
	};

	spehs::vec2 getAxis(Vertex* _vertexArray, const unsigned int _size, const unsigned int _i)
	{
		spehs::vec2 currentVertex(_vertexArray[_i].position.x, _vertexArray[_i].position.y);
		spehs::vec2 nextVertex;
		if (_i + 1 != _size)
			nextVertex = spehs::vec2(_vertexArray[_i + 1].position.x, _vertexArray[_i + 1].position.y);
		else
			nextVertex = spehs::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y);

		spehs::vec2 edgeVector = currentVertex - nextVertex;
		spehs::vec2 normalVector = spehs::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	spehs::vec2 getAxis(spehs::vec2* _vertexArray, const unsigned int _size, const unsigned int _i)
	{
		spehs::vec2 currentVertex = spehs::vec2(_vertexArray[_i].x, _vertexArray[_i].y);
		spehs::vec2 nextVertex;
		if (_i + 1 != _size)
			nextVertex = spehs::vec2(_vertexArray[_i + 1].x, _vertexArray[_i + 1].y);
		else
			nextVertex = spehs::vec2(_vertexArray[0].x, _vertexArray[0].y);

		spehs::vec2 edgeVector = currentVertex - nextVertex;
		spehs::vec2 normalVector = spehs::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	spehs::vec2 getCircleAxis(Vertex* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenter)
	{
		spehs::vec2 polygonVertex = spehs::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y);
		for (int i = 1; i < _size; i++)
		{
			if ((spehs::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y) - _circleCenter).getLength() < (polygonVertex - _circleCenter).getLength())
			{
				polygonVertex = spehs::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y);
			}
		}
		spehs::vec2 edgeVector = polygonVertex - _circleCenter;
		spehs::vec2 normalVector = spehs::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	spehs::vec2 getCircleAxis(spehs::vec2* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenter)
	{
		spehs::vec2 polygonVertex = spehs::vec2(_vertexArray[0].x, _vertexArray[0].y);
		for (int i = 1; i < _size; i++)
		{
			if ((spehs::vec2(_vertexArray[i].x, _vertexArray[i].y) - _circleCenter).getLength() < (polygonVertex - _circleCenter).getLength())
			{
				polygonVertex = spehs::vec2(_vertexArray[i].x, _vertexArray[i].y);
			}
		}
		spehs::vec2 edgeVector = polygonVertex - _circleCenter;
		spehs::vec2 normalVector = spehs::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	
#pragma region BOOL COLLISIONS
	bool SATCollision(Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2)
	{
		spehs::vec2* axis1 = new spehs::vec2[_size1];
		spehs::vec2* axis2 = new spehs::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i).normalize();
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i).normalize();
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
	bool SATCollision(spehs::vec2* _vertexArray1, const unsigned int _size1, spehs::vec2* _vertexArray2, const unsigned int _size2)
	{
		spehs::vec2* axis1 = new spehs::vec2[_size1];
		spehs::vec2* axis2 = new spehs::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i).normalize();
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i).normalize();
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
	bool SATCollision(Vertex* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius)
	{
		spehs::vec2* axis1 = new spehs::vec2[_size];
		spehs::vec2 axis2 = (getCircleAxis(_vertexArray, _size, _circleCenterPoint)).normalize();

		//Get all axes
		for (unsigned int i = 0; i < _size; i++)
		{
			axis1[i] = (getAxis(_vertexArray, _size, i)).normalize();
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
	bool SATCollision(spehs::vec2* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius)
	{
		spehs::vec2* axis1 = new spehs::vec2[_size];
		spehs::vec2 axis2 = (getCircleAxis(_vertexArray, _size, _circleCenterPoint)).normalize();

		//Get all axes
		for (unsigned int i = 0; i < _size; i++)
		{
			axis1[i] = (getAxis(_vertexArray, _size, i)).normalize();
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
	bool circleCollision(const spehs::vec2& _circleCenterPoint1, const float _circleRadius1, const spehs::vec2& _circleCenterPoint2, const float _circleRadius2)
	{
		if ((_circleCenterPoint1 - _circleCenterPoint2).getLength() < (_circleRadius1 + _circleRadius2))
		{
			return true;
		}
		return false;
	}

	bool rayCollision(const spehs::vec2 _rayPosition, const float _rayDirection, const spehs::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance)
	{
		spehs::vec2 endPoint(_rayPosition.x + _rayDistance * cos(_rayDirection), _rayPosition.y + _rayDistance * sin(_rayDirection));
		if ((endPoint - _rayPosition).dot(_circleCenterPoint - _rayPosition) < 0.0f)
		{
			return false;
		}
		if (pointToLineDistance(_rayPosition, endPoint, _circleCenterPoint) <= _circleRadius)
		{
			if ((_rayPosition, _circleCenterPoint).getLength() <= _rayDistance)
			{
				return true;
			}
		}
		return false;
	}
	float rayCollisionDistance(const spehs::vec2 _rayPosition, const float _rayDirection, const spehs::vec2& _circleCenterPoint, const float _circleRadius, const float _rayDistance)
	{
		spehs::vec2 endPoint(_rayPosition.x + _rayDistance * cos(_rayDirection), _rayPosition.y + _rayDistance * sin(_rayDirection));
		float distance = pointToLineDistance(_rayPosition, endPoint, _circleCenterPoint);
		if (distance <= _circleRadius)
		{
			return (_circleCenterPoint - _rayPosition).getLength(); //TEMPs
		}
		return 0.0f;
	}

	bool rayCollision(const spehs::vec2 _rayCastPosition, float _rayDirection, spehs::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance)
	{
		return false;
	}
	float rayCollisionDistance(const spehs::vec2 _rayCastPosition, float _rayDirection, spehs::vec2* _vertexArray, unsigned _vertexArrayLength, const float _rayDistance)
	{
		spehs::vec2 vec(0.0f, 0.0f);
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
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray1, const unsigned int _size1, Vertex* _vertexArray2, const unsigned int _size2)
	{
		float overlap = FLT_MAX;
		spehs::vec2 smallestAxis;
		spehs::vec2* axis1 = new spehs::vec2[_size1];
		spehs::vec2* axis2 = new spehs::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = (getAxis(_vertexArray1, _size1, i)).normalize();
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = (getAxis(_vertexArray2, _size2, i)).normalize();
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
		for (unsigned int i = 0; i < _size1; i++)
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
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}

		spehs::vec2 testPoint;
		for (unsigned int i = 0; i < _size1; i++)
		{
			testPoint = spehs::vec2(_vertexArray1[i].position.x, _vertexArray1[i].position.y);
			//Find which points are colliding
			if (SATCollision(_vertexArray2, _size2, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis2[0]));

				//Find normal
				spehs::vec2 pointVector = testPoint - getCenter(_vertexArray2, _size2);
				for (unsigned i = 1; i < _size2; i++)
				{
					if (pointVector.dot(axis2[i]) > pointVector.dot(deposit.points.back().normal))
					{
						deposit.points.back().normal = axis2[i];
					}
				}
			}
		}

		for (unsigned int i = 0; i < _size2; i++)
		{
			testPoint = spehs::vec2(_vertexArray2[i].position.x, _vertexArray2[i].position.y);
			//Find which points are colliding
			if (SATCollision(_vertexArray1, _size1, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis1[0]));

				//Find normal
				spehs::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray1, _size1);
				for (unsigned i = 1; i < _size1; i++)
				{
					if (pointVector.dot(axis1[i]) > pointVector.dot(deposit.points.back().normal))
					{
						deposit.points.back().normal = axis1[i];
					}
				}
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		for (unsigned int i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal.normalize();
		}

		delete [] axis1;
		delete [] axis2;
		return true;
	}
	bool SATMTVCollision(CollisionResults& deposit, spehs::vec2* _vertexArray1, const unsigned int _size1, spehs::vec2* _vertexArray2, const unsigned int _size2)
	{
		float overlap = FLT_MAX;
		spehs::vec2 smallestAxis;
		spehs::vec2* axis1 = new spehs::vec2[_size1];
		spehs::vec2* axis2 = new spehs::vec2[_size2];

		//Get all axes
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i).normalize();
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i).normalize();
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
		for (unsigned int i = 0; i < _size1; i++)
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
		for (unsigned int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (unsigned int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}

		spehs::vec2 testPoint;
		for (unsigned int i = 0; i < _size1; i++)
		{
			testPoint = spehs::vec2(_vertexArray1[i].x, _vertexArray1[i].y);
			//Find which points are colliding
			if (SATCollision(_vertexArray2, _size2, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis2[0]));

				//Find normal
				spehs::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray2, _size2);
				for (unsigned i = 1; i < _size2; i++)
				{
					if (pointVector.dot(axis2[i]) > pointVector.dot(deposit.points.back().normal))
					{
						deposit.points.back().normal = axis2[i];
					}
				}
			}
		}

		for (unsigned int i = 0; i < _size2; i++)
		{
			testPoint = spehs::vec2(_vertexArray2[i].x, _vertexArray2[i].y);
			//Find which points are colliding
			if (SATCollision(_vertexArray1, _size1, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, axis1[0]));

				//Find normal
				spehs::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray1, _size1);
				for (unsigned i = 1; i < _size1; i++)
				{
					if (pointVector.dot(axis1[i]) > pointVector.dot(deposit.points.back().normal))
					{
						deposit.points.back().normal = axis1[i];
					}
				}
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		for (unsigned int i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal.normalize();
		}

		delete [] axis1;
		delete [] axis2;
		return true;
	}
	bool SATMTVCollision(CollisionResults& deposit, Vertex* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius)
	{
		float overlap = FLT_MAX;
		spehs::vec2 smallestAxis;
		spehs::vec2* axis1 = new spehs::vec2[_size];
		spehs::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i).normalize();
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint).normalize();

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
		for (int i = 0; i < _size; i++)
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
		deposit.MTV = smallestAxis.getNormalized() * abs(overlap);
		
		//Recalculate all axes without normalizing
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);


		spehs::vec2 testPoint;
		for (unsigned int i = 0; i < _size; i++)
		{
			testPoint = spehs::vec2(_vertexArray[i].position.x, _vertexArray[i].position.y);
			//Find which points are colliding
			if (circleCollision(_circleCenterPoint, _circleRadius, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, testPoint - _circleCenterPoint));

				//Find normal
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		spehs::vec2 circlePoint = (getCenter(_vertexArray, _size) - _circleCenterPoint).normalize();
		circlePoint = _circleCenterPoint + circlePoint * _circleRadius;
		//Find which points are colliding
		if (SATCollision(_vertexArray, _size, circlePoint, 0.0f))
		{
			deposit.points.push_back(CollisionResults::Point(circlePoint, axis1[0]));

			//Find normal
			spehs::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray, _size);
			for (unsigned i = 1; i < _size; i++)
			{
				if (pointVector.dot(axis1[i]) > pointVector.dot(deposit.points.back().normal))
				{
					deposit.points.back().normal = axis1[i];
				}
			}
		}

		//Normalize normals
		for (unsigned int i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal.normalize();
		}

		delete [] axis1;
		return true;
	}
	bool SATMTVCollision(CollisionResults& deposit, spehs::vec2* _vertexArray, const unsigned int _size, const spehs::vec2& _circleCenterPoint, const float _circleRadius)
	{
		float overlap = FLT_MAX;
		spehs::vec2 smallestAxis;
		spehs::vec2* axis1 = new spehs::vec2[_size];
		spehs::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i).normalize();
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint).normalize();

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
		for (int i = 0; i < _size; i++)
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
		deposit.MTV = smallestAxis.getNormalized() * abs(overlap);

		//Recalculate all axes without normalizing
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);


		spehs::vec2 testPoint;
		for (unsigned int i = 0; i < _size; i++)
		{
			testPoint = spehs::vec2(_vertexArray[i].x, _vertexArray[i].y);
			//Find which points are colliding
			if (circleCollision(_circleCenterPoint, _circleRadius, testPoint, 0.0f))
			{
				deposit.points.push_back(CollisionResults::Point(testPoint, testPoint - _circleCenterPoint));

				//Find normal
				deposit.points.back().normal = -deposit.points.back().normal; //?
			}
		}

		spehs::vec2 circlePoint = (getCenter(_vertexArray, _size) - _circleCenterPoint).normalize();
		circlePoint = _circleCenterPoint + circlePoint * _circleRadius;
		//Find which points are colliding
		if (SATCollision(_vertexArray, _size, circlePoint, 0.0f))
		{
			deposit.points.push_back(CollisionResults::Point(circlePoint, axis1[0]));

			//Find normal
			spehs::vec2 pointVector = deposit.points.back().position - getCenter(_vertexArray, _size);
			for (unsigned i = 1; i < _size; i++)
			{
				if (pointVector.dot(axis1[i]) > pointVector.dot(deposit.points.back().normal))
				{
					deposit.points.back().normal = axis1[i];
				}
			}
		}

		//Normalize normals
		for (unsigned int i = 0; i < deposit.points.size(); i++)
		{
			deposit.points[i].normal.normalize();
		}

		delete [] axis1;
		return false;
	}
	bool circleMTVCollision(CollisionResults& deposit, const spehs::vec2& _circleCenterPoint1, const float _circleRadius1, const spehs::vec2& _circleCenterPoint2, const float _circleRadius2)
	{
		if ((_circleCenterPoint1 - _circleCenterPoint2).getLength() < (_circleRadius1 + _circleRadius2))
		{
			if (_circleCenterPoint1 == _circleCenterPoint2) //Can't normalize 0 vectors
			{
				deposit.MTV = spehs::vec2(1.0f, 0.0f) * ((_circleCenterPoint1 - _circleCenterPoint2).getLength() - (_circleRadius1 + _circleRadius2));
				deposit.points.push_back(CollisionResults::Point(_circleCenterPoint1 + spehs::vec2(1.0f, 0.0f) * _circleRadius1, deposit.MTV));
			}
			else
			{
				deposit.MTV = (_circleCenterPoint2 - _circleCenterPoint1).getNormalized() * ((_circleCenterPoint1, _circleCenterPoint2).getLength() - (_circleRadius1 + _circleRadius2));
				deposit.points.push_back(CollisionResults::Point(_circleCenterPoint1 + (_circleCenterPoint2 - _circleCenterPoint1).normalize() * _circleRadius1, deposit.MTV));
			}
			return true;
		}
		return false;
	}
#pragma endregion
}