#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

#include "Vertex.h"
#include "ShaderManager.h"


namespace SpehsEngine
{
	enum Shape
	{
		BUTTON = 0,
		RIGHTANGLED_TRIANGLE = 1,
		ISOSCELES_TRIANGLE = 2
	};
	enum DrawMode
	{
		UNDEFINED,
		POLYGON,
		OUTLINE,
	};

	class PolygonBatch;
	class PrimitiveBatch
	{
	public:
		PrimitiveBatch();
		~PrimitiveBatch();
		virtual void draw() = 0;
		virtual void setPosition(float _x, float _y) = 0;
		virtual void setScale(float _newScale){}

		void setRotation(float _newRotAngle);
		void setColor(int _r, int _g, int _b, int _a = 255); //0-255
		void setColor(glm::vec4& col){ color = col; } //0-1
		void setAlpha(int a){ color[3] = a / 255.0f; }
		void setAlpha(float a){ color[3] = a; }
		glm::vec4 getColor(){ return color; }
		void setShader(ShaderName _shaderIndex);
		ShaderName getShaderIndex();

		//true = primitive scales with camera, false = primitive has a scale of 1.0f = 1 pixel
		void setCameraMatrixState(bool _newState);

		//For Polygons only
		virtual void addPolygon(float _xDistanceFromOrigin, float _yDistanceFromOrigin, PolygonBatch* _newPolygon){}
		virtual void setDrawMode(DrawMode _drawMode){}
		virtual void setTexture(unsigned int _textureIndex){}
		virtual void setTextureScale(float _newScale){}
		virtual void resize(float _width, float _height){}
		virtual void updateCollisions(){}
		virtual float getArea(){ return 0.0f; }

		//For Lines only
		void addLine(float _length, float _angle){}

		//For Points only
		virtual void addPoint(float _relativePosX, float _relativePosY){}
		virtual void setTexture(unsigned int _textureIndex, float _pointSize){}

	protected:
		void uniforms();
		void applyTranformations();

		std::vector<GLushort> indices;
		const glm::vec3 rotationVector = glm::vec3(0.0f, 0.0f, 1.0f);

		bool cameraMatrixState = true;
		int numIndices = 0;
		unsigned int textureDataID = 0;
		GLuint vertexArrayObjectID = 0;
		GLuint vertexBufferID = 0;
		GLuint indexBufferID = 0;
		float rotation = 0.0f;
		float scaleX = 1.0f, scaleY = 1.0f;
		glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		glm::mat4 translationMatrix = glm::mat4(1.0f);
		glm::mat4 translationScalingMatrix = glm::mat4(1.0f);
		glm::mat4 transformMatrix = glm::mat4(1.0f);
		glm::mat4 scaledMatrix = glm::mat4(1.0f);
		glm::mat4 scaledRotatedMatrix = glm::mat4(1.0f);
		ShaderName shaderIndex = DefaultPolygon;
		Vertex* vertexArray = nullptr;
	};
}