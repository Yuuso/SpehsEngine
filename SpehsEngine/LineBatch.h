#pragma once
#include "PrimitiveBatch.h"

namespace spehs
{
	class LineBatch : public PrimitiveBatch
	{
	public:
		LineBatch(glm::vec2 _startPoint, glm::vec2 _endPoint);
		~LineBatch();

		void addLine(glm::vec2 _startPoint, glm::vec2 _endPoint);
		void draw();
		void updateVertices(glm::vec2* _vertices, int _numVertices); //Use with caution!
		void setPosition(float _x, float _y);
		void setScale(float _newScale);
		void setThickness(float _thickness);

	private:
		unsigned int numVertices = 0;
		float thickness = 1.0f;
	};
}
