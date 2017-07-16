#pragma once

#include "Vertex.h"
#include "Depth.h"
#include "Color.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <vector>
#include <stdint.h>


//Int based depth that is used to determine the drawing order of 2D primitives.
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
		QUAD = 0x0007,
		QUAD_STRIP = 0x0008,
		POLYGON = 0x0009,

		//Special case for drawing batched polygons with lines
		LINE_TRIANGLE = 0x1003,
	};

	//Base class for polygon, line and point
	class Primitive : public Colorable
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
		void setPosition(const float _x, const float _y);
		void setPosition(const glm::vec2 &_newPosition);
		void setPosition(const Primitive& _other);
		void translate(const float &_x, const float &_y);
		void translate(const glm::vec2& _translation);
		void setScale(const float &_newScale);
		void setScale(const float &_newScaleX, const float &_newScaleY);
		void setScale(const glm::vec2 &_newScale);
		void setScaleX(const float _newScaleX);
		void setScaleY(const float _newScaleX);
		void setRotation(const float &_newRotation);
		void setRotation(const float &_newRotation, const glm::vec3 &_newRotationVector);
		void setRotation(const Primitive& _other);
		void rotate(const float _rotation);
		void setRotationVector(const glm::vec3 &_newRotationVector);
		void setColor(const Primitive& other);
		void setColor(const spehs::Color& _newColor) override;
		void setAlpha(const spehs::Color::Component& _newAlpha) override;
		void setCameraMatrixState(const bool _newState);
		void setPlaneDepth(const PlaneDepth &_newPlaneDepth);
		void setLineWidth(const float &_newWidth);
		void setRenderState(const bool _newState);
		void setRenderState(const Primitive& _other);
		void setShader(const int &_newShaderIndex);
		void setBlending(const bool _newState);

		//Getters
		glm::vec2 getPosition() const { return position; }
		float getX() const { return position.x; }
		float getY() const { return position.y; }
		float getRotation() const { return rotation; }
		bool getCameraMatrixState() const { return cameraMatrixState; }
		bool getRenderState() const { return renderState; }
		PlaneDepth getPlaneDepth() const { return planeDepth; }
		int getShaderIndex() const { return shaderIndex; }
		spehs::Color getColor() const { return primitiveColor; }
		spehs::Color::Component getAlpha() const { return primitiveColor.a; }
		float getScaleX() const { return scaleX; }
		float getScaleY() const { return scaleY; }
		//Public Variables
		std::vector<spehs::Vertex> worldVertexArray; //Transformed vertices

	protected:
		Primitive();
		virtual ~Primitive();

		bool readyForDelete;
		bool blending;
		bool renderState; //Whether Primitive is rendered or not
		bool cameraMatrixState; //Whether camera affects the primitive or not
		bool needUpdate;
		GLuint textureDataID;
		PlaneDepth planeDepth;
		DrawMode drawMode;
		int shaderIndex;
		float lineWidth;
		float rotation;
		float scaleX, scaleY;
		glm::vec3 rotationVector;
		Color primitiveColor;
		glm::mat4 scaledMatrix;
		glm::mat4 scaledRotatedMatrix;

		glm::vec2 position;
		std::vector<spehs::Vertex> vertexArray; //Original vertices

#ifdef _DEBUG //Take a batch manager pointer for improved debug information
	public:
		const BatchManager* myBatchManager;
#endif
	};
}
