
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
	Line* Line::create(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth)
	{
		return getActiveBatchManager()->createLine(_startPoint, _endPoint, _planeDepth);
	}
	Line::Line(const glm::vec2 &_startPoint, const glm::vec2 &_endPoint, const PlaneDepth &_planeDepth)
	{
		//if (glm::distance(_startPoint, _endPoint) < DISTANCE_EPSILON)
			//spehs::console::error("Line needs to have length greater than 0!");

		vertexArray.resize(2);
		vertexArray[0].position = glm::vec2(_startPoint.x, _startPoint.y);
		vertexArray[1].position = glm::vec2(_endPoint.x, _endPoint.y);

		worldVertexArray = vertexArray;

		lineWidth = 1.0f;
		planeDepth = _planeDepth;
		blending = true;
		drawMode = LINE;
	}
	Line::~Line()
	{

	}


	void Line::updateVertices()
	{
		if (needUpdate)
		{
			glm::vec4 vertex;
			scaledMatrix = glm::scale(glm::mat4(), glm::vec3(scaleX, scaleY, 0.0f));
			scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, rotationVector);
			for (unsigned int i = 0; i < worldVertexArray.size(); i++)
			{
				vertex = glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, 0.0f, 1.0f);
				vertex = scaledRotatedMatrix * vertex;
				worldVertexArray[i].position = glm::vec2(vertex.x + position.x, vertex.y + position.y);
			}
			needUpdate = false;
		}
	}

	void Line::setPoints(const glm::vec2 &_newStartPoint, const glm::vec2 &_newEndPoint)
	{
		vertexArray[0].position = glm::vec2(_newStartPoint.x, _newStartPoint.y);
		vertexArray[1].position = glm::vec2(_newEndPoint.x, _newEndPoint.y);
		needUpdate = true;
	}
}
