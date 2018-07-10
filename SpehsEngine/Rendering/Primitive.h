#pragma once

#include "SpehsEngine/Core/Vertex.h"
#include "SpehsEngine/Rendering/Depth.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/Shapes.h"
#include "SpehsEngine/Core/Vector.h"

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

	enum class DrawMode : int
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
		void setPosition(const spehs::vec2& _newPosition);
		void setPosition(const Primitive& _other);
		void translate(const float _x, const float _y);
		void translate(const spehs::vec2& _translation);
		void setScale(const float _newScale);
		void setScale(const float _newScaleX, const float _newScaleY);
		void setScale(const spehs::vec2& _newScale);
		void setScaleX(const float _newScaleX);
		void setScaleY(const float _newScaleX);
		void setRotation(const float _newRotation);
		void setRotation(const float _newRotation, const spehs::vec3& _newRotationVector);
		void setRotation(const Primitive& _other);
		void rotate(const float _rotation);
		void setRotationVector(const spehs::vec3& _newRotationVector);
		void setColor(const Primitive& other);
		void setColor(const spehs::Color& _newColor) override;
		void setAlpha(const spehs::Color::Component& _newAlpha) override;
		void setCameraMatrixState(const bool _newState);
		void setPlaneDepth(const PlaneDepth _newPlaneDepth);
		void setLineWidth(const float _newWidth);
		void setRenderState(const bool _newState);
		void setRenderState(const Primitive& _other);
		void setShaderIndex(const unsigned int _newShaderIndex);
		void setBlending(const bool _newState);

		//Getters
		spehs::vec2 getPosition() const { return position; }
		float getX() const { return position.x; }
		float getY() const { return position.y; }
		float getRotation() const { return rotation; }
		bool getCameraMatrixState() const { return cameraMatrixState; }
		bool getRenderState() const { return renderState; }
		PlaneDepth getPlaneDepth() const { return planeDepth; }
		unsigned int getShaderIndex() const { return shaderIndex; }
		spehs::Color getColor() const { return primitiveColor; }
		spehs::Color::Component getAlpha() const { return primitiveColor.a; }
		float getScaleX() const { return scaleX; }
		float getScaleY() const { return scaleY; }
		//Public Variables
		std::vector<spehs::Vertex> worldVertexArray; //Transformed vertices
		BatchManager& batchManager;

	protected:
		Primitive(BatchManager& batchManager);
		virtual ~Primitive();

		bool readyForDelete = false;
		bool blending = false;
		bool renderState = true; //Whether Primitive is rendered or not
		bool cameraMatrixState = true; //Whether camera affects the primitive or not
		bool needUpdate = true;
		GLuint textureDataID = 0;
		PlaneDepth planeDepth = 0;
		DrawMode drawMode = DrawMode::UNDEFINED;
		unsigned int shaderIndex;
		float lineWidth = 0.0f;
		float rotation = 0.0f;
		float scaleX = 1.0f, scaleY = 1.0f;
		spehs::vec3 rotationVector = spehs::vec3(0.0f, 0.0f, 1.0f);
		Color primitiveColor = Color(255, 255, 255, 255);
		glm::mat4 scaledMatrix = glm::mat4(1.0f);
		glm::mat4 scaledRotatedMatrix = glm::mat4(1.0f);
		spehs::vec2 position = spehs::vec2(0.0f, 0.0f);
		std::vector<spehs::Vertex> vertexArray; //Original vertices

	};
}
