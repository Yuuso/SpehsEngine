#pragma once

#include "SpehsEngine/Rendering/Primitive.h"


typedef unsigned int GLuint;

namespace se
{
	namespace rendering
	{
		class Line : public Primitive
		{
			friend class BatchManager;
		public:

			Line* getLinePtr() override { return this; }

			void updateVertices();
			void setPoints(const glm::vec2& _newStartPoint, const glm::vec2& _newEndPoint);
			//void setPoints(const float &_newLength, const float &_newAngle);

		protected:
			Line(BatchManager& batchManager, const PlaneDepth& _planeDepth);
			Line(BatchManager& batchManager, const glm::vec2& _startPoint, const glm::vec2& _endPoint, const PlaneDepth& _planeDepth);
			//Line(const float &_length, const float &_angle); //Starting point is at 0,0
			~Line();
		};
	}
}
