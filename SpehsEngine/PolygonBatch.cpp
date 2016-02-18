#define GLM_FORCE_RADIANS
#define PI 3.14159265f
#define HALF_PI 1.57079632679f
#define TWO_PI 6.28318530718f

#define SCALE_C 0.001f

#include "PolygonBatch.h"
#include "Shapes.h"
#include "Exceptions.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "ApplicationData.h"
#include "Time.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace SpehsEngine
{
	PolygonBatch::PolygonBatch(int _shapeID, float _width, float _height)
	{
		int N;
		if (_shapeID >= 3) //Regular Convex Polygons
		{
			numPolygons++;
			N = _shapeID;
			float firstPosition;

			Vertex* pos = new Vertex[N];
			//firstPosition adjusts initial the rotation for even numbered polygons
			//Initial rotation is set so that the "lowest" (bottom) line is drawn horizontally
			if (N % 2)
				firstPosition = HALF_PI;
			else
				firstPosition = HALF_PI + (TWO_PI / N) / 2;
			pos[0].setPosition(cos(firstPosition), sin(firstPosition));
			float minX = pos[0].position.x, minY = pos[0].position.y, maxX = pos[0].position.x, maxY = pos[0].position.y;
			for (int i = 1; i < N; i++)
			{
				//Set position
				pos[i].setPosition(cos(firstPosition - i * (TWO_PI / N)), sin(firstPosition - i * (TWO_PI / N)));

				//Check min/max
				if (pos[i].position.x > maxX)
					maxX = pos[i].position.x;
				else if (pos[i].position.x < minX)
					minX = pos[i].position.x;
				if (pos[i].position.y > maxY)
					maxY = pos[i].position.y;
				else if (pos[i].position.y < minY)
					minY = pos[i].position.y;
			}
			float width = maxX - minX;//Note: Max x IS positive, min IS negative
			float height = maxY - minY;//Note: Max x IS positive, min IS negative
			float wFactor = 1.0f / width;
			float hFactor = 1.0f / height;

			for (int i = 0; i < N; i++)
			{
				pos[i].position.x *= wFactor;
				pos[i].position.y *= hFactor;
			}

			vertexArray = pos;
			worldVertexArray = new Vertex[N];
			std::copy(pos, pos + N, stdext::checked_array_iterator<Vertex*>(worldVertexArray, N));
			originalVertexArray = new Vertex[N];
			std::copy(pos, pos + N, stdext::checked_array_iterator<Vertex*>(originalVertexArray, N));

			numVertices = new GLuint[numPolygons];
			numVertices[numPolygons - 1] = N;
			for (int i = 0; i < numPolygons; i++)
			{
				totalNumVertices += numVertices[i];
			}
		}
		else //Polygons from Shapes
		{
			numPolygons++;

			N = getNumVertices(SpehsEngine::Shape(_shapeID));
			if (N < 3)
				fatalError("Can't create a polygon with less than 3 vertices!");

			Vertex* shapeVertexArray = getShapeVertexArray(SpehsEngine::Shape(_shapeID));
			if (shapeVertexArray == nullptr)
				fatalError("Vertex Array is a nullptr!");

			numVertices = new GLuint[numPolygons];
			numVertices[numPolygons - 1] = N;
			for (int i = 0; i < numPolygons; i++)
			{
				totalNumVertices += numVertices[i];
			}

			//Copy the shape vertex array into polygon's own arrays
			vertexArray = new Vertex[N];
			std::copy(shapeVertexArray, shapeVertexArray + N, stdext::checked_array_iterator<Vertex*>(vertexArray, N));
			worldVertexArray = new Vertex[N];
			std::copy(shapeVertexArray, shapeVertexArray + N, stdext::checked_array_iterator<Vertex*>(worldVertexArray, N));
			originalVertexArray = new Vertex[N];
			std::copy(shapeVertexArray, shapeVertexArray + N, stdext::checked_array_iterator<Vertex*>(originalVertexArray, N));
			delete []shapeVertexArray;
		}

		//Set texture coordinates
		for (unsigned int i = 0; i < N; i++)
		{
			vertexArray[i].uv.u = (vertexArray[i].position.x + 1.0f) / 2.0f;
			vertexArray[i].uv.v = (vertexArray[i].position.y + 1.0f) / 2.0f;
		}

		setPolygonIndices(POLYGON);
		resize(_width, _height);
		//NOTE: RESIZE CONTAINTS UPDATEVBO SO IT'S NOT NEEDED AGAIN HERE!
	}
	PolygonBatch::PolygonBatch(Vertex* _vertexData, int _numVertices, float _width, float _height)
	{
		numPolygons++;
		int N = _numVertices;
		if (N < 3)
			fatalError("Can't create a polygon with less than 3 vertices!");

		if (_vertexData == nullptr)
			fatalError("Vertex Array is a nullptr!");

		numVertices = new GLuint[numPolygons];
		numVertices[numPolygons - 1] = N;
		for (int i = 0; i < numPolygons; i++)
		{
			totalNumVertices += numVertices[i];
		}

		//Copy the shape vertex array into polygon's own arrays
		vertexArray = new Vertex[N];
		std::copy(_vertexData, _vertexData + N, stdext::checked_array_iterator<Vertex*>(vertexArray, N));
		worldVertexArray = new Vertex[N];
		std::copy(_vertexData, _vertexData + N, stdext::checked_array_iterator<Vertex*>(worldVertexArray, N));
		originalVertexArray = new Vertex[N];
		std::copy(_vertexData, _vertexData + N, stdext::checked_array_iterator<Vertex*>(originalVertexArray, N));

		//Set texture coordinates
		for (unsigned int i = 0; i < N; i++)
		{
			vertexArray[i].uv.u = (vertexArray[i].position.x + 1.0f) / 2.0f;
			vertexArray[i].uv.v = (vertexArray[i].position.y + 1.0f) / 2.0f;
		}

		setPolygonIndices(POLYGON);

		resize(_width, _height);
		//NOTE: RESIZE CONTAINTS UPDATEVBO SO IT'S NOT NEEDED AGAIN HERE!
	}
	PolygonBatch::~PolygonBatch()
	{
		if (vertexArray != nullptr)
			delete [] vertexArray;
		if (worldVertexArray != nullptr)
			delete [] worldVertexArray;
		if (originalVertexArray != nullptr)
			delete [] originalVertexArray;
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &indexBufferID);
		glDeleteVertexArrays(1, &vertexArrayObjectID);
	}


	void PolygonBatch::addPolygon(float _xDistanceFromOrigin, float _yDistanceFromOrigin, PolygonBatch* _newPolygon)
	{
		this->numPolygons += _newPolygon->numPolygons;
		//move vertices of the new polygon
		for (unsigned int i = 0; i < _newPolygon->totalNumVertices; i++)//Should the whole polygon be re-centered here?
		{
			_newPolygon->vertexArray[i].position.x += _xDistanceFromOrigin;
			_newPolygon->vertexArray[i].position.y += _yDistanceFromOrigin;
		}
		//create new combined vertex array
		Vertex* pos = new Vertex[this->totalNumVertices + _newPolygon->totalNumVertices + 1];
		for (unsigned int i = 0; i < this->totalNumVertices; i++)
		{
			pos[i] = this->vertexArray[i];
		}
		for (unsigned int i = this->totalNumVertices; i < this->totalNumVertices + _newPolygon->totalNumVertices; i++)
		{
			pos[i] = _newPolygon->vertexArray[i - this->totalNumVertices];
		}
		delete [] this->vertexArray; //delete the old vertexArray
		this->vertexArray = pos;
		this->totalNumVertices += _newPolygon->totalNumVertices;

		//Create new numVertices array
		GLuint* newNumVertices = new GLuint[numPolygons];
		for (int i = 0; i < _newPolygon->numPolygons; i++)//transfer from new polygon
		{
			newNumVertices[numPolygons - (1 + i)] = _newPolygon->numVertices[_newPolygon->numPolygons - (1 + i)];
		}
		for (int i = 0; i < numPolygons - _newPolygon->numPolygons; i++)//transfer from this polygon
		{
			newNumVertices[i] = numVertices[i];
		}
		delete [] numVertices;//delete old array
		numVertices = newNumVertices;

		//create new combined original vertex array
		Vertex* pos2 = new Vertex[this->totalNumVertices + _newPolygon->totalNumVertices + 1];
		for (unsigned int i = 0; i < this->totalNumVertices; i++)
		{
			pos2[i] = this->originalVertexArray[i];
		}
		for (unsigned int i = this->totalNumVertices; i < this->totalNumVertices + _newPolygon->totalNumVertices; i++)
		{
			pos2[i] = _newPolygon->originalVertexArray[i - this->totalNumVertices];
		}
		delete [] this->originalVertexArray; //delete the old vertexArray
		this->originalVertexArray = pos2;

		delete _newPolygon;

		//Set texture coordinates
		for (unsigned int i = 0; i < totalNumVertices; i++)
		{
			vertexArray[i].uv.u = (vertexArray[i].position.x + 1.0f) / 2.0f;
			vertexArray[i].uv.v = (vertexArray[i].position.y + 1.0f) / 2.0f;
		}

		setPolygonIndices(POLYGON);

		updateVBO();
	}

	void PolygonBatch::draw()
	{
		glEnable(GL_BLEND);

		applyTranformations();

		shaderManager->use(shaderIndex);

		uniforms();

		glBindVertexArray(vertexArrayObjectID);//What's really needed here...?

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, uv));


		switch (drawMode)
		{
		case OUTLINE:
		{
			glLineWidth(1.0f);
			glDrawElements(GL_LINE_LOOP, numIndices, GL_UNSIGNED_SHORT, (GLvoid*) 0);
			drawCalls++;
			vertexDrawCount += numIndices;
			break;
		}
		case POLYGON:
		{
			glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, (GLvoid*) 0);
			drawCalls++;
			vertexDrawCount += numIndices;
			break;
		}
		default:
		{
			unexpectedError("Undefined polygon drawMode!");
			break;
		}
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		shaderManager->unuse(shaderIndex);
	}


	void PolygonBatch::resize(float _width, float _height)
	{
		for (int i = 0; i < totalNumVertices; i++)
		{
			vertexArray[i].position.x = originalVertexArray[i].position.x * _width;
			vertexArray[i].position.y = originalVertexArray[i].position.y * _height;
		}

		for (int i = 0; i < numVertices[0]; i++)
		{
			worldVertexArray[i].position.x = originalVertexArray[i].position.x * _width;
			worldVertexArray[i].position.y = originalVertexArray[i].position.y * _height;
		}

		updateVBO();
	}


	void PolygonBatch::setPosition(float _x, float _y)
	{
		worldPosition.setPosition(_x, _y);
		translationMatrix = glm::translate(glm::mat4(), glm::vec3(_x / ((float) applicationData->getWindowWidth() / (float) applicationData->getWindowHeight()), _y, 0.0f));
	}


	void PolygonBatch::setScale(float _newScale)
	{
		scaleX = _newScale * SCALE_C;
		scaleY = _newScale * SCALE_C;
	}


	void PolygonBatch::setDrawMode(DrawMode _drawMode)
	{
		if (drawMode != _drawMode)
			setPolygonIndices(_drawMode);
	}


	void PolygonBatch::setTexture(std::string _texturePath)
	{
		textureDataID = textureManager->preloadTexture(_texturePath);
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
	}
	void PolygonBatch::setTexture(size_t _textureID)
	{
		textureDataID = _textureID;
		if (shaderIndex == DefaultPolygon)
			shaderIndex = DefaultTexture;
		shaderManager->getShader(shaderIndex)->uniforms->textureDataID = textureDataID;
	}


	void PolygonBatch::setUVScale(float _newScale)
	{
		for (unsigned int i = 0; i < totalNumVertices; i++)
		{
			vertexArray[i].uv.u *= _newScale;
			vertexArray[i].uv.v *= _newScale;
		}

		updateVBO();
	}


	void PolygonBatch::updateCollisions()
	{
		if (worldVertexArray != nullptr)
		{
			static glm::vec4 vertex;
			scaledMatrix = glm::scale(glm::mat4(), glm::vec3(scaleX, scaleY, 0.0f));
			scaledRotatedMatrix = glm::rotate(scaledMatrix, rotation, rotationVector);
			for (unsigned int i = 0; i < numVertices[0]; i++)
			{
				vertex = glm::vec4(vertexArray[i].position.x, vertexArray[i].position.y, 0.0f, 1.0f);
				vertex = scaledRotatedMatrix * vertex;
				worldVertexArray[i].setPosition(vertex.x + worldPosition.position.x, vertex.y + worldPosition.position.y);
			}
		}
	}


	float PolygonBatch::getArea()
	{
		float area = 0.0f;
		int j = numVertices[0] - 1;
		for (int i = 0; i < numVertices[0]; i++)
		{
			area += (worldVertexArray[j].position.x * worldVertexArray[i].position.y) - (worldVertexArray[i].position.x * worldVertexArray[j].position.y);
			j = i;
		}
		return abs((area / 2));
	}


	float PolygonBatch::getRadius()
	{
		if (radius > 0.000001f) //If it has been calculated already
		{
			return radius;
		}

		float max = glm::distance(glm::vec2(0.0f, 0.0f), glm::vec2(vertexArray[0].position.x, vertexArray[0].position.y));
		for (unsigned i = 1; i < numVertices[0]; i++)
		{
			if (glm::distance(glm::vec2(0.0f, 0.0f), glm::vec2(vertexArray[i].position.x, vertexArray[i].position.y)) > max)
			{
				max = glm::distance(glm::vec2(0.0f, 0.0f), glm::vec2(vertexArray[i].position.x, vertexArray[i].position.y));
			}
		}
		radius = max;
		return radius;
	}


	//Private:
	void PolygonBatch::setPolygonIndices(DrawMode _drawMode)
	{
		int a = 0;
		numIndices = 0;
		switch (_drawMode)
		{
		case OUTLINE://TODO: fix
		{
			numIndices = totalNumVertices;
			for (int k = 0; k < numPolygons; k++)
			{
				for (unsigned short i = 0; a < numIndices;)
				{
					indices.push_back((GLushort) i++);
					a++;
				}
			}
			drawMode = OUTLINE;
			break;
		}
		case POLYGON:
		{
			unsigned short firstIndexOfPolygon = 0;
			for (int k = 0; k < numPolygons; k++)
			{
				numIndices += ((numVertices[k] - 2) * 3);
				for (unsigned short i = 1; a < numIndices;)
				{
					indices.push_back((GLushort) (firstIndexOfPolygon));
					indices.push_back((GLushort) (firstIndexOfPolygon + i++));
					indices.push_back((GLushort) (firstIndexOfPolygon + i));
					a += 3;
				}
				firstIndexOfPolygon += (unsigned short) (3 + (numVertices[k] - 3));
			}
			drawMode = POLYGON;
			break;
		}
		default:
			fatalError("Cannot set polygon indices -> polygon draw mode not supported!");
		}

		if (indexBufferID != 0)
			glDeleteBuffers(1, &indexBufferID);
		glGenBuffers(1, &indexBufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		indices.clear();
	}


	void PolygonBatch::updateVBO()
	{
		//Reset radius
		radius = 0.0f;

		if (vertexBufferID != 0)
			glDeleteBuffers(1, &vertexBufferID);//There's probably a better way to do this.
		if (vertexArrayObjectID = 0)
			glGenVertexArrays(1, &vertexArrayObjectID);

		glBindVertexArray(vertexArrayObjectID);
		glGenBuffers(1, &vertexBufferID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * totalNumVertices, vertexArray, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}