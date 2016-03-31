#pragma once

#include "Vertex.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <stdint.h>


//Int based depth that is used to determine the drawing order of 2D primitives.
typedef int16_t PlaneDepth;
typedef unsigned int GLuint;

namespace spehs
{
	class Polygon;
	class Line;
	class Point;
	class BatchManager;
	class PrimitiveBatch;

	enum Shape
	{
		BUTTON = 0,
		RIGHTANGLED_TRIANGLE = 1,
		ISOSCELES_TRIANGLE = 2
	};
	enum DrawMode : int
	{
		UNDEFINED = -1,

		POINT = 0x0000,
		LINE = 0x0001,
		LINE_LOOP = 0x0002,
		LINE_STRIP = 0x0003,
		LINE_ADJACENCY = 0x000A,
		LINE_STRIP_ADJACENCY = 0x000B,
		TRIANGLE = 0x0004,
		TRIANGLE_FAN = 0x0006,
		TRIANGLE_STRIP = 0x0005,
		TRIANGLE_ADJACENCY = 0x000C,
		TRIANGLE_STRIP_ADJACENCY = 0x000D,
		PATCH = 0xE,

		OUTLINE = LINE_LOOP,
		POLYGON = TRIANGLE,
	};

	//Base class for polygon, line and point
	class Primitive
	{
		friend class BatchManager;
		friend class PrimitiveBatch;

	public:
		virtual Polygon* getPolygonPtr(){ return nullptr; }
		virtual Line* getLinePtr(){ return nullptr; }
		virtual Point* getPointPtr(){ return nullptr; }
		
		virtual void updateVertices() = 0; //This is called automatically when rendering

		void destroy(); //Primitives can only be deleted by BatchManager, user can request deletion by calling destroy()
		//NOTE: BatchManager does clean up the primitives after it is deleted, but it is advised that the user destroys primitives that are not needed

		//Setters
		void setPosition(const float &_x, const float &_y);
		void setPosition(const float &_x, const float &_y, const float &_z);
		void setPosition(const glm::vec2 &_newPosition);
		void setPosition(const glm::vec3 &_newPosition);
		void setPosition(const Position &_newPosition);
		void setScale(const float &_newScale);
		void setScale(const float &_newScaleX, const float &_newScaleY);
		void setScale(const glm::vec2 &_newScale);
		void setRotation(const float &_newRotation);
		void setRotation(const float &_newRotation, const glm::vec3 &_newRotationVector);
		void setRotationVector(const glm::vec3 &_newRotationVector);
		void setColor(const glm::vec4 &_newColor);
		void setColor(const glm::vec3 &_newColor);
		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a = 1.0f);
		void setColorAlpha(const float &_a);
		void setColorAlpha(const unsigned char &_a);
		void setCameraMatrixState(const bool _newState);
		void setPlaneDepth(const PlaneDepth &_newPlaneDepth);
		void setLineWidth(const float &_newWidth);
		void setRenderState(const bool _newState);
		void setShader(const int &_newShaderIndex);
		void setBlending(const bool _newState);

		//Getters
		bool getCameraMatrixState(){ return cameraMatrixState; }
		PlaneDepth getPlaneDepth(){ return planeDepth; }
		int getShaderIndex(){ return shaderIndex; }
		glm::vec4 getColor(){ return primitiveColor; }

		//Public Variables
		Vertex* worldVertexArray; //Transformed vertices
		GLuint numVertices;

	protected:
		Primitive();
		virtual ~Primitive();

		bool readyForDelete;
		bool blending;
		bool renderState; //Whether Primitive is rendered or not
		bool cameraMatrixState; //Whether camera affects the primitive or not
		GLuint textureDataID;
		PlaneDepth planeDepth;
		DrawMode drawMode;
		int shaderIndex;
		float lineWidth;
		float rotation;
		float scaleX, scaleY;
		glm::vec3 rotationVector;
		glm::vec4 primitiveColor;
		glm::mat4 scaledMatrix;
		glm::mat4 scaledRotatedMatrix;

		Position position;
		Vertex* vertexArray; //Original vertices
	};
}
