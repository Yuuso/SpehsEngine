#include "SATCollision.h"
#include "Vertex.h"
#include "glm\vec2.hpp"
#include "glm\matrix.hpp"


namespace spehs
{
	Projection projectPolygon(glm::vec2 _axis, Vertex* _vertexArray, int _size)
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
	Projection projectPolygon(glm::vec2 _axis, Position* _vertexArray, int _size)
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
	Projection projectCircle(glm::vec2 _axis, glm::vec2& _vertex, float _radius)
	{
		float min = glm::dot(_axis, glm::vec2(_vertex.x, _vertex.y));
		float max = min + _radius;
		min -= _radius;
		Projection returnProj(min, max);
		return returnProj;
	};

	glm::vec2 getAxis(Vertex* _vertexArray, int _size, int _i)
	{
		glm::vec2 currentVertex = glm::vec2(_vertexArray[_i].position.x, _vertexArray[_i].position.y);
		glm::vec2 nextVertex;
		if (_i + 1 != _size)
			nextVertex = glm::vec2(_vertexArray[_i + 1].position.x, _vertexArray[_i + 1].position.y);
		else
			nextVertex = glm::vec2(_vertexArray[0].position.x, _vertexArray[0].position.y);

		glm::vec2 edgeVector = currentVertex - nextVertex;
		glm::vec2 normalVector = glm::vec2(-edgeVector.y, edgeVector.x);
		return normalVector;
	};
	glm::vec2 getAxis(Position* _vertexArray, int _size, int _i)
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
	glm::vec2 getCircleAxis(Vertex* _vertexArray, int _size, glm::vec2& _circleCenter)
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
	glm::vec2 getCircleAxis(Position* _vertexArray, int _size, glm::vec2& _circleCenter)
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


	bool SATCollision(Vertex* _vertexArray1, int _size1, Vertex* _vertexArray2, int _size2)
	{
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}
		//Normalize all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(axis2[i]);
		}

		//Loop through axes2
		for (int i = 0; i < _size2; i++)
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
		for (int i = 0; i < _size1; i++)
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
	bool SATCollision(Position* _vertexArray1, int _size1, Position* _vertexArray2, int _size2)
	{
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}
		//Normalize all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(axis2[i]);
		}

		//Loop through axes2
		for (int i = 0; i < _size2; i++)
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
		for (int i = 0; i < _size1; i++)
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
	glm::vec2 SATMTVCollision(Vertex* _vertexArray1, int _size1, Vertex* _vertexArray2, int _size2)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}
		//Normalize all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(axis2[i]);
		}

		//Loop through axes2
		for (int i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return glm::vec2(NULL);
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
		for (int i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return glm::vec2(NULL);
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
		delete [] axis1;
		delete [] axis2;
		return glm::normalize(smallestAxis) * abs(overlap);
	}
	glm::vec2 SATMTVCollision(Position* _vertexArray1, int _size1, Position* _vertexArray2, int _size2)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size1];
		glm::vec2* axis2 = new glm::vec2[_size2];

		//Get all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = getAxis(_vertexArray1, _size1, i);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = getAxis(_vertexArray2, _size2, i);
		}
		//Normalize all axes
		for (int i = 0; i < _size1; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		for (int i = 0; i < _size2; i++)
		{
			axis2[i] = glm::normalize(axis2[i]);
		}

		//Loop through axes2
		for (int i = 0; i < _size2; i++)
		{
			Projection p1 = projectPolygon(axis2[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis2[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return glm::vec2(NULL);
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
		for (int i = 0; i < _size1; i++)
		{
			Projection p1 = projectPolygon(axis1[i], _vertexArray1, _size1);
			Projection p2 = projectPolygon(axis1[i], _vertexArray2, _size2);
			if (!p1.overlap(p2))
			{
				delete axis1;
				delete axis2;
				return glm::vec2(NULL);
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
		delete [] axis1;
		delete [] axis2;
		return glm::normalize(smallestAxis) * abs(overlap);
	}


	bool SATCollision(Vertex* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius)
	{
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);

		//Normalize all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		axis2 = glm::normalize(axis2);

		//Go through axes2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return false;
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
		}
		//Here we know that no separating axis was found and there is a collision
		delete [] axis1;
		return true;
	}
	bool SATCollision(Position* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius)
	{
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);

		//Normalize all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		axis2 = glm::normalize(axis2);

		//Go through axes2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return false;
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
		}
		//Here we know that no separating axis was found and there is a collision
		delete [] axis1;
		return true;
	}
	glm::vec2 SATMTVCollision(Vertex* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);

		//Normalize all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		axis2 = glm::normalize(axis2);

		//Go through axes2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return glm::vec2(NULL);
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
				return glm::vec2(NULL);
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
		delete [] axis1;
		return glm::normalize(smallestAxis) * abs(overlap);
	}
	glm::vec2 SATMTVCollision(Position* _vertexArray, int _size, glm::vec2& _circleCenterPoint, float _circleRadius)
	{
		float overlap = FLT_MAX;
		glm::vec2 smallestAxis;
		glm::vec2* axis1 = new glm::vec2[_size];
		glm::vec2 axis2;

		//Get all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = getAxis(_vertexArray, _size, i);
		}
		axis2 = getCircleAxis(_vertexArray, _size, _circleCenterPoint);

		//Normalize all axes
		for (int i = 0; i < _size; i++)
		{
			axis1[i] = glm::normalize(axis1[i]);
		}
		axis2 = glm::normalize(axis2);

		//Go through axes2
		Projection p1 = projectPolygon(axis2, _vertexArray, _size);
		Projection p2 = projectCircle(axis2, _circleCenterPoint, _circleRadius);
		if (!p1.overlap(p2))
		{
			delete axis1;
			return glm::vec2(NULL);
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
				return glm::vec2(NULL);
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
		delete [] axis1;
		return glm::normalize(smallestAxis) * abs(overlap);
	}


	bool CircleCollision(glm::vec2& _circleCenterPoint1, float _circleRadius1, glm::vec2& _circleCenterPoint2, float _circleRadius2)
	{
		if (glm::distance(_circleCenterPoint1, _circleCenterPoint2) < (_circleRadius1 + _circleRadius2))
		{
			return true;
		}
		return false;
	}
	glm::vec2 CircleMTVCollision(glm::vec2& _circleCenterPoint1, float _circleRadius1, glm::vec2& _circleCenterPoint2, float _circleRadius2)
	{
		if (glm::distance(_circleCenterPoint1, _circleCenterPoint2) < (_circleRadius1 + _circleRadius2))
		{
			return glm::normalize(-_circleCenterPoint1 + _circleCenterPoint2) * (glm::distance(_circleCenterPoint1, _circleCenterPoint2) - (_circleRadius1 + _circleRadius2));
		}
		return glm::vec2(NULL);
	}
}