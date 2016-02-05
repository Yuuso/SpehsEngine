#include "LineBatch.h"
#include "Exceptions.h"
#include "ShaderManager.h"
#include "ApplicationData.h"
#include "Time.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>


namespace SpehsEngine
{
	LineBatch::LineBatch(glm::vec2 _startPoint, glm::vec2 _endPoint)
	{
		if (glm::distance(_startPoint, _endPoint) <= 0.0f)
			fatalError("Line needs to have length greater than 0!");

		numVertices = 2;
		Vertex* pos = new Vertex[numVertices];
		pos[0].setPosition(_startPoint.x, _startPoint.y);
		pos[1].setPosition(_endPoint.x, _endPoint.y);

		indices.push_back(0);
		indices.push_back(1);
		vertexArray = pos;

		glGenVertexArrays(1, &vertexArrayObjectID);
		glBindVertexArray(vertexArrayObjectID);

		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertexArray, GL_STATIC_DRAW);

		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		indices.clear();
	}
	LineBatch::~LineBatch()
	{
		if (vertexArray != nullptr)
			delete [] vertexArray;
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteVertexArrays(1, &vertexArrayObjectID);
	}


	void LineBatch::addLine(glm::vec2 _startPoint, glm::vec2 _endPoint)
	{
		if (glm::distance(_startPoint, _endPoint) <= 0.0f)
			fatalError("Line needs to have length greater than 0!");

		numVertices += 2;
		Vertex* pos = new Vertex[numVertices];
		for (unsigned int i = 0; i < numVertices - 2; i++)
		{
			pos[i] = vertexArray[i];
		}
		pos[numVertices - 2].setPosition(_startPoint.x, _startPoint.y);
		pos[numVertices - 1].setPosition(_endPoint.x, _endPoint.y);
		delete [] vertexArray;
		vertexArray = pos;

		for (unsigned int i = 0; i < numVertices; i++)
		{
			indices.push_back(i);
		}

		glBindVertexArray(vertexArrayObjectID);

		glDeleteBuffers(1, &vertexBufferID);
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, pos, GL_STATIC_DRAW);

		glDeleteBuffers(1, &indexBufferID);
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		indices.clear();
	}


	void LineBatch::draw()
	{
		glEnable(GL_LINE_SMOOTH);

		applyTranformations();

		shaderManager->use(shaderIndex);

		uniforms();

		glBindVertexArray(vertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

		glLineWidth(thickness);
		glDrawElements(GL_LINES, numVertices, GL_UNSIGNED_SHORT, (GLvoid*) 0);
		drawCalls++;
		vertexDrawCount += numVertices;

		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		shaderManager->unuse(shaderIndex);
	}


	void LineBatch::updateVertices(glm::vec2* _vertices, int _numVertices)
	{
		if (_numVertices != numVertices)
			fatalError("Incorrect number of vertices!");

		Vertex* pos = new Vertex[numVertices];
		for (unsigned int i = 0; i < numVertices; i++)
		{
			pos[i].setPosition(_vertices[i].x, _vertices[i].y);
		}
		delete [] vertexArray;
		vertexArray = pos;

		for (unsigned int i = 0; i < numVertices; i++)
		{
			indices.push_back(i);
		}

		//Update buffers
		glBindVertexArray(vertexArrayObjectID);

		glDeleteBuffers(1, &vertexBufferID);
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, pos, GL_STATIC_DRAW);

		glDeleteBuffers(1, &indexBufferID);
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		indices.clear();
	}


	void LineBatch::setPosition(float _x, float _y)
	{
		translationMatrix = glm::translate(glm::mat4(), glm::vec3(_x / ((float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight()), _y, 0.0f));
	}


	void LineBatch::setScale(float _newScale)
	{
		scaleX = _newScale;
		scaleY = _newScale;
	}


	void LineBatch::setThickness(float _thickness)
	{
		thickness = _thickness;
	}
}
