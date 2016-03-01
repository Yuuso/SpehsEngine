#pragma once

#include "BatchObject.h"
#include "Vertex.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>


//Int based depth that is used to determine the drawing order of 2D primitives.
typedef int16_t PlaneDepth;

namespace SpehsEngine
{
	class Polygon;
	class Line;
	class Point;

	enum Shape
	{
		BUTTON = 0,
		RIGHTANGLED_TRIANGLE = 1,
		ISOSCELES_TRIANGLE = 2
	};
	enum DrawMode
	{
		UNDEFINED = 0,
		POLYGON = 1,
		OUTLINE = 2,
	};

	//Base class for polygon, line and point
	class Primitive : public BatchObject
	{
	public:
		Primitive();

		Primitive* getPrimitivePtr(){ return this; }
		virtual Polygon* getPolygonPtr(){ return nullptr; }
		virtual Line* getLinePtr(){ return nullptr; }
		virtual Point* getPointPtr(){ return nullptr; }

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
		void setRotation(const float &_newRotation, const glm::vec3 &_newRotationVector = glm::vec3(0.0f, 0.0f, 1.0f));
		void setColor(const glm::vec4 &_newColor);
		void setColor(const glm::vec3 &_newColor);
		void setColor(const unsigned char &_r, const unsigned char &_g, const unsigned char &_b, const unsigned char &_a = 1.0f);
		void setColorAlpha(const float &_a);
		void setColorAlpha(const unsigned char &_a);
		void setCameraMatrixState(const bool &_newState);
		void setPlaneDepth(const PlaneDepth &_newPlaneDepth);

		//Getters
		PlaneDepth getPlaneDepth(){ return planeDepth; }

	protected:
		virtual ~Primitive();

		bool cameraMatrixState; //Whether primitive scales with camera or not
		PlaneDepth planeDepth;
		float rotation;
		float scaleX, scaleY;
		glm::vec3 rotationVector;
		glm::vec4 primitiveColor;
	};
}
