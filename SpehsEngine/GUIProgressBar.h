#pragma once
#include "GUIRectangle.h"


namespace spehs
{
	class GUIProgressBar : public GUIRectangle
	{
	public:
		GUIProgressBar();
		~GUIProgressBar();

		void updateMinSize() override;
		void updateScale() override;
		void updatePosition() override;

		void setProgressBarFrontColor(const glm::vec3& color);
		void setProgressBarFrontColor(const glm::vec4& color);
		void setProgressBarFrontColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		void setProgressBarFrontColor(const float r, const float g, const float b, const float a = 1.0f);
		void setProgressBarFrontColorAlpha(const unsigned char a);
		void setProgressBarFrontColorAlpha(const float a);
		void setProgressBarBackColor(const glm::vec3& color);
		void setProgressBarBackColor(const glm::vec4& color);
		void setProgressBarBackColor(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 255);
		void setProgressBarBackColor(const float r, const float g, const float b, const float a = 1.0f);
		void setProgressBarBackColorAlpha(const unsigned char a);
		void setProgressBarBackColorAlpha(const float a);

	private:
		spehs::Polygon* progressBarFront;
		spehs::Polygon* progressBarBack;
	};
}