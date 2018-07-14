#pragma once

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Rendering/Depth.h"
#include <glm/vec2.hpp>

namespace se
{
	namespace rendering
	{
		class BatchManager;
		class Line;
		class Polygon;

		class Arrow : public Colorable
		{
		public:
			Arrow(BatchManager& batchManager, const int16_t planeDepth = 0);
			Arrow(BatchManager& batchManager, const glm::vec2& _startPoint, const glm::vec2& _endPoint, const int16_t planeDepth = 0);
			~Arrow();

			void setPosition(const glm::vec2& _startPoint, const glm::vec2& _endPoint);
			void setLineThickness(const float _thickness);
			void setArrowPointerSize(const float _pointerWidth, const float _pointerHeight);
			void setArrowPointerSize(const glm::vec2& _ref);
			void setColor(const se::Color& _color) override;
			void setAlpha(const float _alpha) override;
			void setRenderState(const bool _value);
			void setDepth(const PlaneDepth _value);
			void setCameraMatrixState(const bool _value);

		private:
			Line* linePart;
			Polygon* polygonPart;
			float pointerWidth;
			float pointerHeight;
			float length;//Length from start to end
		};
	}
}