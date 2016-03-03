
#include "Line.h"
#include "Console.h"
#include "BatchManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>

#define GLM_FORCE_RADIANS


namespace spehs
{
	Line::Line(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth)
	{
		if (glm::distance(_startPoint, _endPoint) < DISTANCE_EPSILON)
			console->error("Line needs to have length greater than 0!");

		numVertices = 2;
		Vertex* pos = new Vertex[numVertices];
		pos[0].setPosition(_startPoint.x, _startPoint.y);
		pos[1].setPosition(_endPoint.x, _endPoint.y);

		vertexArray = pos;
		worldVertexArray = new Vertex[numVertices];
		std::copy(pos, pos + numVertices, stdext::checked_array_iterator<Vertex*>(worldVertexArray, numVertices));

		lineWidth = 1.0f;
		planeDepth = _planeDepth;
		blending = true;
	}
	Line::Line(const glm::vec3 &_startPoint, const glm::vec3 &_endPoint)
	{
		if (glm::distance(_startPoint, _endPoint) < DISTANCE_EPSILON)
			console->error("Line needs to have length greater than 0!");

		numVertices = 2;
		Vertex* pos = new Vertex[numVertices];
		pos[0].setPosition(_startPoint.x, _startPoint.y, _startPoint.z);
		pos[1].setPosition(_endPoint.x, _endPoint.y, _endPoint.z);

		vertexArray = pos;
		worldVertexArray = new Vertex[numVertices];
		std::copy(pos, pos + numVertices, stdext::checked_array_iterator<Vertex*>(worldVertexArray, numVertices));

		lineWidth = 1.0f;
		blending = false;
	}
	Line::~Line()
	{

	}


	void Line::updateVertices()
	{
		static glm::vec4 vertex;
		scaledMatrix = glm::scale(glm::mat4(), glm::vec3(scaleX, scaleY, 0.0f));
		scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, rotationVector);
		for (unsigned int i = 0; i < numVertices; i++)
		{
			vertex = glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, vertexArray[i].position.z, 1.0f);
			vertex = scaledRotatedMatrix * vertex;
			worldVertexArray[i].setPosition(vertex.x + position.x, vertex.y + position.y, vertex.z + position.z);
		}
	}

	void Line::setPoints(const glm::vec2 &_newStartPoint, const glm::vec2 &_newEndPoint)
	{
		vertexArray[0].setPosition(_newStartPoint.x, _newStartPoint.y);
		vertexArray[1].setPosition(_newEndPoint.x, _newEndPoint.y);
	}

	void Line::setPoints(const glm::vec3 &_newStartPoint, const glm::vec3 &_newEndPoint)
	{
		vertexArray[0].setPosition(_newStartPoint.x, _newStartPoint.y, _newStartPoint.z);
		vertexArray[1].setPosition(_newEndPoint.x, _newEndPoint.y, _newEndPoint.z);
	}
}
