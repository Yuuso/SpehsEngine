#include "PointBatch.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "ApplicationData.h"
#include "Time.h"
#include "OpenGLError.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>


namespace SpehsEngine
{
	PointBatch::PointBatch()
	{
		Vertex* pos = new Vertex[1];
		pos[0].setPosition(0.0f, 0.0f);

		vertexArray = pos;
		numIndices = 1; //This is numVertices now...

		glGenVertexArrays(1, &vertexArrayObjectID);
		glBindVertexArray(vertexArrayObjectID);

		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 2, pos, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}
	PointBatch::~PointBatch()
	{
		if (vertexArray != nullptr)
			delete [] vertexArray;
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteVertexArrays(1, &vertexArrayObjectID);
	}

	void PointBatch::draw()
	{
		glEnable(GL_BLEND);
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_PROGRAM_POINT_SIZE);

		applyTranformations();

		shaderManager->use(shaderIndex);

		uniforms();

		glBindVertexArray(vertexArrayObjectID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));

		glDrawArrays(GL_POINTS, 0, numIndices);
		drawCalls++;
		vertexDrawCount += numIndices;

		glDisableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		shaderManager->unuse(shaderIndex);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}


	void PointBatch::setPosition(float _x, float _y)
	{
		translationMatrix = glm::translate(glm::mat4(), glm::vec3(_x / ((float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight()), _y, 0.0f));
	}


	void PointBatch::addPoint(float _relativePosX, float _relativePosY)
	{
		auto newPoint = PointBatch();
		//move vertices of the new point
		for (int i = 0; i < newPoint.numIndices; i++)
		{
			newPoint.vertexArray[i].position.x += _relativePosX;
			newPoint.vertexArray[i].position.y += _relativePosY;
		}
		//create new combined vertex array
		Vertex* pos = new Vertex[this->numIndices + 1];
		for (int i = 0; i < this->numIndices; i++)
		{
			pos[i] = this->vertexArray[i];
		}
		for (int i = this->numIndices; i < this->numIndices + newPoint.numIndices; i++)
		{
			pos[i] = newPoint.vertexArray[i - this->numIndices];
		}
		delete this->vertexArray; //delete the old vertexArray
		this->vertexArray = pos;
		this->numIndices += newPoint.numIndices;

		//update vertex buffer data
		if (vertexBufferID != 0)
			glDeleteBuffers(1, &vertexBufferID);
		glGenBuffers(1, &vertexBufferID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numIndices, vertexArray, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

#ifdef _DEBUG
		checkOpenGLErrors(__FILE__, __LINE__);
#endif
	}


	void PointBatch::setScale(float _newScale)
	{
		scaleX = _newScale;
		scaleY = _newScale;
	}


	void PointBatch::setTexture(std::string _texturePath)
	{
		textureDataID = textureManager->preloadTexture(_texturePath);
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
	}
	void PointBatch::setTexture(size_t _textureID)
	{
		textureDataID = _textureID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
	}
}