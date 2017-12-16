
#include "SpehsEngine/Rendering/Line.h"
#include "SpehsEngine/Rendering/Console.h"
#include "SpehsEngine/Rendering/BatchManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>


namespace spehs
{
	Line::Line(const PlaneDepth& _planeDepth)
	{
		vertexArray.resize(2);
		vertexArray[0].position = spehs::vec2::zero;
		vertexArray[1].position = spehs::vec2::zero;

		worldVertexArray = vertexArray;

		lineWidth = 1.0f;
		planeDepth = _planeDepth;
		blending = true;
		drawMode = LINE;
	}

	Line::Line(const spehs::vec2& _startPoint, const spehs::vec2& _endPoint, const PlaneDepth& _planeDepth)
	{
		vertexArray.resize(2);
		vertexArray[0].position = spehs::vec2(_startPoint.x, _startPoint.y);
		vertexArray[1].position = spehs::vec2(_endPoint.x, _endPoint.y);

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
			scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, glm::vec3(rotationVector.x, rotationVector.y, rotationVector.z));
			for (unsigned int i = 0; i < worldVertexArray.size(); i++)
			{
				vertex = glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, 0.0f, 1.0f);
				vertex = scaledRotatedMatrix * vertex;
				worldVertexArray[i].position = spehs::vec2(vertex.x + position.x, vertex.y + position.y);
			}
			needUpdate = false;
		}
	}

	void Line::setPoints(const spehs::vec2& _newStartPoint, const spehs::vec2& _newEndPoint)
	{
		vertexArray[0].position = spehs::vec2(_newStartPoint.x, _newStartPoint.y);
		vertexArray[1].position = spehs::vec2(_newEndPoint.x, _newEndPoint.y);
		needUpdate = true;
	}
}
